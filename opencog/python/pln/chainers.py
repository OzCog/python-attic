__author__ = 'jade'

from pln.rules.rules import Rule
from pln.logic import Logic
from pln.formulas import revisionFormula

from opencog.atomspace import types, Atom, AtomSpace, TruthValue

import random
from collections import defaultdict

# @todo The AtomSpace has an ImportanceIndex which is much more efficient. This algorithm checks
# every Atom's STI (in the whole AtomSpace)
def get_attentional_focus(atomspace, attentional_focus_boundary=0):
    nodes = atomspace.get_atoms_by_type(types.Atom)
    attentional_focus = []
    for node in nodes:
        if node.av['sti'] > attentional_focus_boundary:
            attentional_focus.append(node)
    return attentional_focus

class AbstractChainer(Logic):
    '''Has important utility methods for chainers.'''
    def __init__(self, atomspace):
        Logic.__init__(self, atomspace)
        self.rules = []

    def add_rule(self, rule):
        assert isinstance(rule, Rule)
        assert type(rule) != Rule

        self.rules.append(rule)

    # Finds a list of candidate atoms and then matches all of them against the template.
    # Uses the Attentional Focus where possible but will search the whole AtomSpace if necessary.
    def _select_one_matching(self, template, s = {}, allow_zero_tv = False):
        # If the template is a specific atom, just return that!
        if len(self.variables(template)) == 0:
            return template

        # TODO backward chaining doesn't work, because this method will pick up Atoms
        # that have variables in them (including the queries left over from other inferences)!!!

        attentional_focus = get_attentional_focus(self._atomspace)

        atom = self._select_from(template, s, attentional_focus, allow_zero_tv, useAF=True)

        if not atom:
            # if it can't find anything in the attentional focus, try the whole atomspace.
            if template.type == types.VariableNode:
                root_type = types.Atom
            else:
                root_type = template.type
            all_atoms = self._atomspace.get_atoms_by_type(root_type)
            atom = self._select_from(template, s, all_atoms, allow_zero_tv, useAF=False)

        return atom

    def _select_from(self, template, substitution, atoms, allow_zero_tv, useAF):
        # never allow inputs with variables. (not even for backchaining targets)
        ground_results = True

        atoms = self.find(template, atoms, substitution)

        if not allow_zero_tv:
            atoms = [atom for atom in atoms if atom.tv.count > 0]

        if ground_results:
            atoms = [atom for atom in atoms if len(self.variables(atom)) == 0]

        if len(atoms) == 0:
            return None

        if useAF:
            return self._selectOne(atoms)
        else:
            # selectOne doesn't work if the STI is below 0 i.e. outside of the attentional focus.
            return random.choice(atoms)

    def _selectOne(self, atoms):
        # The score should always be an int or stuff will get weird. sti is an int but TV mean and conf are not
        def sti_score(atom):
            return atom.av['sti']

        def mixed_score(atom):
            return int(sti_score(atom)*atom.tv.mean*atom.tv.confidence)

        score = sti_score

        assert type(atoms[0]) == Atom

        max = sum([score(atom) for atom in atoms])
        pick = random.randrange(0, max)
        current = 0
        for atom in atoms:
            current += score(atom)
            if current >= pick:
                return atom

        assert False

    def _select_rule(self):
        if not self.learnRuleFrequencies:
            return random.choice(self.rules)
        else:
            def score(rule):
                return self.rule_count[rule]

            max = sum([score(rule) for rule in self.rules])
            pick = random.randrange(0, max)
            current = 0
            for rule in self.rules:
                current += score(rule)
                if current >= pick:
                    self.rule_count[rule] += 1
                    return rule

    def valid_structure(self, atom):
        '''Does a kind of 'type-check' to see if an Atom's structure makes sense.
           The forward chainer is very creative and will come up with anything allowed by the Rules
           otherwise.'''
        if atom.type in [types.InheritanceLink, types.SubsetLink]:
            #is_between_nodes = atom.out[0].is_node() and atom.out[1].is_node()
            not_self_link    = atom.out[0] != atom.out[1]
            return not_self_link
        else:
            return True

    def log_failed_inference(self,message):
        print 'Attempted invalid inference:',message

class InferenceHistoryIndex(object):
    def __init__(self):
        self.rule_to_io = {}

    def record_new_application(self, rule, inputs, outputs):
        outputs = tuple(outputs)
        inputs = tuple(inputs)

        try:
            output_to_inputs = self.rule_to_io[rule]
        except KeyError:
            output_to_inputs = self.rule_to_io[rule] = defaultdict(set)
            assert output_to_inputs != None

        input_tuple_set = output_to_inputs[outputs]

        if inputs in input_tuple_set:
            return False
        else:
            input_tuple_set.add(inputs)

    def lookup_all_applications(self, rule, outputs):
        try:
            output_to_inputs = self.rule_to_io[rule]
        except KeyError:
            return []
        input_tuple_set = output_to_inputs[outputs]
        return input_tuple_set

class Chainer(AbstractChainer):
    '''PLN forward/backward chainer. It chooses Atoms randomly based on STI. It can do a single
       backward or forward step. By running the step repeatedly it can simulate chaining, without
       needing a messy specialized chaining algorithm. It can do a mixture of forward and backward
       chaining and easily interoperate with other OpenCog processes.

       It lets you use any Rule frequencies (and can learn them). You should set stimulateAtoms=True
       if you want it to create chains (otherwise it will just make shallow inferences based on a whole bunch
       of Atoms, which is sometimes good too).

       It can check for repeated inferences and cycles. PLN truth value revision is supported.'''

    ### public interface

    def __init__(self, atomspace, stimulateAtoms=False, agent=None, learnRuleFrequencies=False):
        AbstractChainer.__init__(self, atomspace)

        # It stores a reference to the MindAgent object so it can stimulate atoms.
        self._stimulateAtoms = stimulateAtoms
        self._agent = agent

        self.atomspace = atomspace

        # For every atom, store the atoms used to produce it (including the atoms used to produce them).
        # This prevents cycles (very important) as well as repeating the same inference.
        # Map from Atom -> set(Atom)
        # Default value is the empty set
        self.trails = defaultdict(set)
        #self.produced_from = defaultdict(set)
        self.history_index = InferenceHistoryIndex()

        self.history_atomspace = AtomSpace()
        # TODO actually load and save these. When loading it, rebuild the indexes above.

        # Record how often each Rule is used. To bias the Rule frequencies.
        # It will take longer to adapt if you set this higher (this is important so it won't
        # get a crazy feedback loop).
        initial_frequency = 100

        def constant_factory():
            return initial_frequency
        if learnRuleFrequencies:
            self.learnRuleFrequencies = True
            self.rule_count = defaultdict(constant_factory)

    def forward_step(self):
        rule = self._select_rule()

        results = self._apply_forward(rule)

        return results

    def backward_step(self):
        rule = self._select_rule()

        results = self._apply_backward(rule)

        return results

    ### forward chaining implementation

    def _apply_forward(self, rule):
        # randomly choose suitable atoms for this rule's inputs
            # choose a random atom matching the first input to the rule
            # choose a random atom matching the second input to the rule, compatible with the first input
            # etc
            # it can fail if there are no compatible atoms
        # if all inputs are found, then
            # apply the rule and create the output
            # give it an appropriate TV (using the formula and possibly revision)
            # give it an STI boost
            # record this inference in the InferenceHistoryRepository

        (generic_inputs, generic_outputs) = rule.standardize_apart_input_output(self)
        specific_inputs = []
        empty_substitution = {}
        subst = self._choose_inputs(specific_inputs, generic_inputs, empty_substitution)
        if subst is None:
            return None
        # set the outputs after you've found all the inputs
        # mustn't use '=' because it will discard the original reference and thus have no effect
        specific_outputs = self.substitute_list(subst, generic_outputs)

        # handle rules that create their output in a custom way, not just using templates
        if hasattr(rule, 'custom_compute'):
            (specific_outputs, output_tvs) = rule.custom_compute(specific_inputs)
            return self._apply_rule(rule, specific_inputs, specific_outputs, output_tvs, revise=True)
        elif hasattr(rule, 'temporal_compute'):
            # All inputs ever, and then use the special temporal computation instead of revision.
            all_input_tuples = self.history.lookup_all_applications(rule, specific_outputs)
            (specific_outputs, output_tvs) = rule.temporal_compute(all_input_tuples)
            return self._apply_rule(rule, specific_inputs, specific_outputs, output_tvs, revise=False)
        else:
            if not self._validate(rule, specific_inputs, specific_outputs):
                self.log_failed_inference('bogus truth value or 0 count')
                return None
            output_tvs = rule.calculate(specific_inputs)
            if output_tvs is None:
                return None
            return self._apply_rule(rule, specific_inputs, specific_outputs, output_tvs, revise=True)

    def _choose_inputs(self, return_inputs, input_templates, subst_so_far, allow_zero_tv=False):
        '''Find suitable inputs and outputs for a Rule. Chooses them at random based on STI. Store them in return_inputs and return_outputs (lists of Atoms). Return the substitution if inputs were found, None otherwise.'''
        return_inputs += [x for x in input_templates]

        for i in xrange(0, len(input_templates)):
            template = input_templates[i]

            atom = self._select_one_matching(template, subst_so_far)
            
            if atom != None:
                # Find the substitution that would change 'template' to 'atom'
                # Alternatively this could be returned by _select_one_matching
                subst_so_far = self.unify(template, atom, subst_so_far)
                if subst_so_far == None:
                    import pdb; pdb.set_trace()

                return_inputs[i] = atom
            else:
                if not allow_zero_tv:
                    print 'unable to match:',template
                    return None
                # This means it won't be able to produce the output, but choosing some inputs is still essential for backward chaining.
                # Just specialize the rest of the inputs. These "input" will actually just be 0-tv atoms, and it can become a BC target later.
                #query = self.substitute(subst_so_far, template)
                #return_inputs[i] = query
                return_inputs[i:] = self.substitute_list(subst_so_far, input_templates[i:])
                return subst_so_far

        return subst_so_far

    ### backward chaining implementation

    def _apply_backward(self, rule):
        # randomly choose suitable atoms for this rule's inputs
            # choose a random atom matching the first input to the rule
            # choose a random atom matching the second input to the rule, compatible with the first input
            # etc
            # it can fail if there are no compatible atoms
        # if all inputs are found, then
            # apply the rule and create the output
            # give it an appropriate TV (using the formula and possibly revision)
            # give it an STI boost
            # record this inference in the InferenceHistoryRepository

        (generic_inputs, generic_outputs) = rule.standardize_apart_input_output(self)
        specific_outputs = []
        empty_substitution = {}
        subst = self._choose_outputs(specific_outputs, generic_outputs, empty_substitution)

        if not subst:
            return None

        specific_inputs = []
        subst = self._choose_inputs(specific_inputs, generic_outputs, subst, allow_zero_tv = True)

        print rule, map(str,specific_outputs), map(str,specific_inputs)

        # If it doesn't find suitable inputs, then it can still stimulate the atoms, but not assign a TruthValue
        # Stimulating the inputs makes it more likely to find them in future.

        # some of the validations might not make sense for backward chaining
        if not self._validate(rule, specific_inputs, specific_outputs):
            return None

        if self._all_nonzero_tvs(specific_inputs):
            output_tvs = rule.calculate(specific_inputs)

            return self._apply_rule(rule, specific_inputs, specific_outputs, output_tvs)
        else:
            if self._stimulateAtoms:
#                for atom in specific_outputs:
#                    self._give_stimulus(atom)
                for atom in specific_inputs:
                    self._give_stimulus(atom)
            return (specific_outputs, specific_inputs)

    def _choose_outputs(self, return_outputs, output_templates, subst_so_far):
        return_inputs = [x for x in output_templates]

        for i in xrange(0, len(output_templates)):
            template = output_templates[i]

            atom = self._select_one_matching(template, subst_so_far, allow_zero_tv=True)
            if atom is None:
                self.log_failed_inference('backward chainer: unable to find target atom matching:'+str(template))
                return None

            # Find the substitution that would change 'template' to 'atom'
            # Alternatively this could be returned by _select_one_matching
            subst_so_far = self.unify(template, atom, subst_so_far)
            if subst_so_far == None:
                import pdb; pdb.set_trace()

            return_outputs[i] = atom

        return subst_so_far

    ### stuff used by both forward and backward chaining

    def _apply_rule(self, rule, inputs, outputs, output_tvs, revise=True):
        if revise:
            assert isinstance(output_tvs, list)

            for (atom, new_tv) in zip(outputs, output_tvs):
                self._revise_tvs(atom, new_tv)

        if self._stimulateAtoms:
            for atom in outputs:
                self._give_stimulus(atom)
            for atom in inputs:
                self._give_stimulus(atom)

        return (rule, inputs, outputs)

    def _revise_tvs(self, atom, new_tv):
        old_tv = atom.tv

        revised_tv = revisionFormula([old_tv, new_tv])
        atom.tv = revised_tv

    def _give_stimulus(self, atom):
        # Arbitrary
        STIMULUS_PER_ATOM = 10
        self._agent.stimulate_atom(atom, STIMULUS_PER_ATOM)

    ### automatically reject some inferences based on various problems

    def _validate(self, rule, inputs, outputs):
        print rule, map(str,inputs), map(str,outputs)
        # Sanity checks
        if not self.valid_structure(outputs[0]):
            self.log_failed_inference('invalid structure')
            return False

        if self._compute_trail_and_check_cycles(outputs[0], inputs):
            self.log_failed_inference('cycle detected')
            return False

        if self._is_repeated(rule, outputs, inputs):
            self.log_failed_inference('repeated inference')
            return False

        return True

    def _compute_trail_and_check_cycles(self, output, inputs):
        ''' Recursively find the atoms used to produce output (the inference trail). If there is a cycle, return True.
            Otherwise return False'''
        trail = self.trails[output]

        # Check for cycles before adding anything into the trails
        if output in inputs:
            return True
        for atom in inputs:
            input_trail = self.trails[atom]

            if output in input_trail:
                return None

        for atom in inputs:
            # Reusing the same Atom more than once is bad
            # (maybe? What if you're combining it with a different atom to produce a new TV for the same result?)
            #if atom in trail:
            #    return None
            trail.add(atom)

        for atom in inputs:
            input_trail = self.trails[atom]
            trail |= input_trail

        return False

    def _is_repeated(self, rule, outputs, inputs):
        # Record the exact list of atoms used to produce an output one time. (Any atom can be
        # produced multiple ways using different Rules and inputs.)
        # Return True if this exact inference has been applied before

        # In future this should record to the Inference History Repository atomspace
        # convert the inputs to a tuple so they can be stored in a set.

        # About unordered links
        # Such as And(A B) or And(B A)
        # The AtomSpace will create only one representation of each unordered link.
        # So this algorithm should still work okay for unordered links.

        # TODO it should work for Rules where the input order doesn't matter (e.g. AndRule)
        # currently it won't notice repetition for different input orders

        new = self.history_index.record_new_application(rule, inputs=inputs, outputs=outputs)
        if not new:
            return False
        else:
            self._add_to_inference_repository(rule, outputs, inputs)

    def _add_to_inference_repository(self, rule, outputs, inputs):
        TA = self.history_atomspace
        L = TA.add_link
        N = TA.add_node

        # create new lists of inputs and outputs for the separate history atomspace
        inputs  = [self.transfer_atom(TA, a) for a in inputs]
        outputs = [self.transfer_atom(TA, a) for a in outputs]

        L(types.ExecutionLink, [
            N(types.GroundedSchemaNode, rule.name),
            L(types.ListLink, [
                L(types.ListLink, inputs),
                L(types.ListLink, outputs)
            ])
        ])

    def load_inference_repository(self):
        # TODO fill the history atomspace
        TA = self.history_atomspace

        for link in TA.get_atoms_by_type(types.ExecutionLink):
            rule_name = link.out[0]
            list_link = link.out[1]
            inputs= list_link.out[0]
            outputs= list_link.out[1]

            rule = self.lookup_rule(rule_name)
            inputs  = [self.transfer_atom(self.atomspace, a) for a in inputs]
            outputs = [self.transfer_atom(self.atomspace, a) for a in outputs]

    def lookup_rule(self, rule_name):
        for rule in self.rules:
            if rule.name == rule_name:
                return rule

        raise ValueError("lookup_rule: rule doesn't exist "+rule_name)

