; opencog/embodiment/scm/predicates-frames.scm
;
; Copyright (C) 2009 Novamente LLC
; All Rights Reserved
; Author(s): Samir Araujo
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU Affero General Public License v3 as
; published by the Free Software Foundation and including the exceptions
; at http://opencog.org/wiki/Licenses
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU Affero General Public License
; along with this program; if not, write to:
; Free Software Foundation, Inc.,
; 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


; Embodiment make use of several AtomTable structures to represent
; perception signals reveived by its sensors, via Proxy.
; Each perception is represented using an EvaluationLink
; and a PredicateNode.
;
; In order to make the Embodiment to use NLP to comunicate
; with an avatar, all these perceptions are needed. However,
; RelEx, the module used to convert sentences said
; by the avatars to Atoms, use Frames to represent what
; was said. So, we need to convert all the Embodiment 
; perceptions into Frames to make them compatible with RelEx.
;
; This files contains a list of all Frames structures, used 
; to represent the Embodiment Perceptions.


; Definition of Frame Social_behavior_evaluation
; #Gradable_attributes <- #Social_behavior_evaluation
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Social_behavior_evaluation")
   (DefinedFrameNode "#Gradable_attributes")
)

; #Social_behavior_evaluation:Behavior
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Social_behavior_evaluation")
   (DefinedFrameElementNode "#Social_behavior_evaluation:Behavior")
)
; #Social_behavior_evaluation:Individual
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Social_behavior_evaluation")
   (DefinedFrameElementNode "#Social_behavior_evaluation:Individual")
)
; #Social_behavior_evaluation:Judge
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Social_behavior_evaluation")
   (DefinedFrameElementNode "#Social_behavior_evaluation:Judge")
)


; Definition of Frame Morality_evaluation

; #Social_behavior_evaluation <- #Morality_evaluation
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Morality_evaluation")
   (DefinedFrameNode "#Social_behavior_evaluation")
)

; #Morality_evaluation:Evaluee
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Morality_evaluation")
   (DefinedFrameElementNode "#Morality_evaluation:Evaluee")
)
; #Morality_evaluation:Expressor
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Morality_evaluation")
   (DefinedFrameElementNode "#Morality_evaluation:Expressor")
)



; Definition of Frame Emotion_directed

; #Emotion_directed:Event
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Emotion_directed")
   (DefinedFrameElementNode "#Emotion_directed:Event")
)
; #Emotion_directed:Experiencer
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Emotion_directed")
   (DefinedFrameElementNode "#Emotion_directed:Experiencer")
)
; #Emotion_directed:Expressor
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Emotion_directed")
   (DefinedFrameElementNode "#Emotion_directed:Expressor")
)
; #Emotion_directed:Stimulus
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Emotion_directed")
   (DefinedFrameElementNode "#Emotion_directed:Stimulus")
)
; #Emotion_directed:State
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Emotion_directed")
   (DefinedFrameElementNode "#Emotion_directed:State")
)
; #Emotion_directed:Degree
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Emotion_directed")
   (DefinedFrameElementNode "#Emotion_directed:Degree")
)
; #Emotion_directed:Manner
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Emotion_directed")
   (DefinedFrameElementNode "#Emotion_directed:Manner")
)
; #Emotion_directed:Empathy_target
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Emotion_directed")
   (DefinedFrameElementNode "#Emotion_directed:Empathy_target")
)
; #Emotion_directed:Reason
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Emotion_directed")
   (DefinedFrameElementNode "#Emotion_directed:Reason")
)


; Definition of Frame Entity

; #Entity:Entity
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Entity")
   (DefinedFrameElementNode "#Entity:Entity")
)
; #Entity:Name
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Entity")
   (DefinedFrameElementNode "#Entity:Name")
)
; #Entity:Type
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Entity")
   (DefinedFrameElementNode "#Entity:Type")
)

; Definition of Frame Questioning

; #Questioning:Addressee
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Questioning")
   (DefinedFrameElementNode "#Questioning:Addressee")
)
; #Questioning:Message
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Questioning")
   (DefinedFrameElementNode "#Questioning:Message")
)
; #Questioning:Speaker
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Questioning")
   (DefinedFrameElementNode "#Questioning:Speaker")
)
; #Questioning:Manner
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Questioning")
   (DefinedFrameElementNode "#Questioning:Manner")
)

; Definition of Frame Request

; #Request:Message
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Request")
   (DefinedFrameElementNode "#Request:Message")
)

; #Request:Addressee
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Request")
   (DefinedFrameElementNode "#Request:Addressee")
)


; Definition of Frame Relation

; #Relation:Entity1
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Relation")
   (DefinedFrameElementNode "#Relation:Entity1")
)

; #Relation:Entity2
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Relation")
   (DefinedFrameElementNode "#Relation:Entity2")
)

; #Relation:Relation_type
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Relation")
   (DefinedFrameElementNode "#Relation:Relation_type")
)


; Definition of Frame Trajector-Landmark

; #Relation <- #Trajector-Landmark
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Trajector-Landmark")
   (DefinedFrameNode "#Relation")
)

; #Trajector-Landmark:Landmark
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Trajector-Landmark")
   (DefinedFrameElementNode "#Trajector-Landmark:Landmark")
)

; #Trajector-Landmark:Profiled_region
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Trajector-Landmark")
   (DefinedFrameElementNode "#Trajector-Landmark:Profiled_region")
)

; #Trajector-Landmark:Trajector 
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Trajector-Landmark")
   (DefinedFrameElementNode "#Trajector-Landmark:Trajector")
)



; Definition of Frame Locative_relation

; #Trajector-Landmark <- #Locative_relation
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Locative_relation")
   (DefinedFrameNode "#Trajector-Landmark")
)

; #State <- #Locative_relation
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Locative_relation")
   (DefinedFrameNode "#State")
)

; #Trajector-Landmark <- #Locative_relation
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Locative_relation")
   (DefinedFrameNode "#Trajector-Landmark")
)


; #Locative_relation:Figure
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Locative_relation")
   (DefinedFrameElementNode "#Locative_relation:Figure")
)

; #Locative_relation:Ground
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Locative_relation")
   (DefinedFrameElementNode "#Locative_relation:Ground")
)

; an adaptation of the original frame to handle two grounds (between)
; #Locative_relation:Ground_2
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Locative_relation")
   (DefinedFrameElementNode "#Locative_relation:Ground_2")
)


; Definition of Frame Moving_in_place

; #Moving_in_place:Theme
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Moving_in_place")
   (DefinedFrameElementNode "#Moving_in_place:Theme")
)

; #Moving_in_place:Direction
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Moving_in_place")
   (DefinedFrameElementNode "#Moving_in_place:Direction")
)

; #Moving_in_place:Angle
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Moving_in_place")
   (DefinedFrameElementNode "#Moving_in_place:Angle")
)

; #Moving_in_place:Fixed_location
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Moving_in_place")
   (DefinedFrameElementNode "#Moving_in_place:Fixed_location")
)


; Definition of Frame Substance

; #Physical_entity <- #Substance
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Substance")
   (DefinedFrameNode "#Physical_entity")
)


; Definition of Frame Transitive_action

; #Event <- #Transitive_action
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Transitive_action")
   (DefinedFrameNode "#Event")
)

; #Objective_influence <- #Transitive_action
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Transitive_action")
   (DefinedFrameNode "#Objective_influence")
)


; Definition of Frame Intentionally_act

; #Event <- #Intentionally_act
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Intentionally_act")
   (DefinedFrameNode "#Event")
)

; #Transitive_action <- #Intentionally_act
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Intentionally_act")
   (DefinedFrameNode "#Transitive_action")
)



; Definition of Frame Intentionally_affect

; #Intentionally_act <- #Intentionally_affect
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Intentionally_affect")
   (DefinedFrameNode "#Intentionally_act")
)



; Definition of Frame Ingestion

; #Ingest_substance <- #Ingestion
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Ingestion")
   (DefinedFrameNode "#Ingest_substance")
)

; #Manipulation <- #Ingestion
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Ingestion")
   (DefinedFrameNode "#Manipulation")
)

; #Ingestion:Ingestible
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Ingestion")
   (DefinedFrameElementNode "#Ingestion:Ingestible")
)


; Definition of Frame Food

; #Physical_entity <- #Food
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Food")
   (DefinedFrameNode "#Physical_entity")
)


; Definition of Frame Motion

; #Motion:Theme
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Motion")
   (DefinedFrameElementNode "#Motion:Theme")
)

; #Motion:Direction
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Motion")
   (DefinedFrameElementNode "#Motion:Direction")
)


; Definition of Frame Dimension

; #Dimension:Dimension
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Dimension")
   (DefinedFrameElementNode "#Dimension:Dimension")
)

; #Dimension:Object 
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Dimension")
   (DefinedFrameElementNode "#Dimension:Object")
)

; #Dimension:Measurement 
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Dimension")
   (DefinedFrameElementNode "#Dimension:Measurement")
)



; Definition of Frame Gradable_attributes

; #Gradable_attributes:Attribute
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Gradable_attributes")
   (DefinedFrameElementNode "#Gradable_attributes:Attribute")
)

; #Gradable_attributes:Degree
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Gradable_attributes")
   (DefinedFrameElementNode "#Gradable_attributes:Degree")
)

; #Gradable_attributes:Value
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Gradable_attributes")
   (DefinedFrameElementNode "#Gradable_attributes:Value")
)




; Definition of Frame Position_on_a_scale

; #Gradable_attributes <- #Position_on_a_scale
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Position_on_a_scale")
   (DefinedFrameNode "#Gradable_attributes")
)

; #Position_on_a_scale:Value
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Position_on_a_scale")
   (DefinedFrameElementNode "#Position_on_a_scale:Value")
)



; Definition of Frame Evaluative_comparison

; #Position_on_a_scale <- #Evaluative_comparison
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Evaluative_comparison")
   (DefinedFrameNode "#Position_on_a_scale")
)


; #Evaluative_comparison:Attribute 
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Evaluative_comparison")
   (DefinedFrameElementNode "#Evaluative_comparison:Attribute")
)

; #Evaluative_comparison:Profiled_attribute
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Evaluative_comparison")
   (DefinedFrameElementNode "#Evaluative_comparison:Profiled_attribute")
)

; #Evaluative_comparison:Profiled_item
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Evaluative_comparison")
   (DefinedFrameElementNode "#Evaluative_comparison:Profiled_item")
)

; #Evaluative_comparison:Standard_attribute
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Evaluative_comparison")
   (DefinedFrameElementNode "#Evaluative_comparison:Standard_attribute")
)

; #Evaluative_comparison:Standard_item
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Evaluative_comparison")
   (DefinedFrameElementNode "#Evaluative_comparison:Standard_item")
)



; Definition of Frame Make_noise

; #Make_noise:Noisy_event
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Make_noise")
   (DefinedFrameElementNode "#Make_noise:Noisy_event")
)

; #Make_noise:Sound
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Make_noise")
   (DefinedFrameElementNode "#Make_noise:Sound")
)

; #Make_noise:Sound_source
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Make_noise")
   (DefinedFrameElementNode "#Make_noise:Sound_source")
)

; #Make_noise:Degree
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Make_noise")
   (DefinedFrameElementNode "#Make_noise:Degree")
)

; #Make_noise:Iterations
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Make_noise")
   (DefinedFrameElementNode "#Make_noise:Iterations")
)



; Definition of Frame Possession

; #Possession:Owner
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Possession")
   (DefinedFrameElementNode "#Possession:Owner")
)

; #Possession:Possession
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Possession")
   (DefinedFrameElementNode "#Possession:Possession")
)



; Definition of Frame Education_teaching

; #Intentionally_act <- #Education_teaching
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Education_teaching")
   (DefinedFrameNode "#Intentionally_act")
)


; #Education_teaching:Course
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Education_teaching")
   (DefinedFrameElementNode "#Education_teaching:Course")
)

; #Education_teaching:Fact
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Education_teaching")
   (DefinedFrameElementNode "#Education_teaching:Fact")
)

; #Education_teaching:Institution
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Education_teaching")
   (DefinedFrameElementNode "#Education_teaching:Institution")
)

; #Education_teaching:Material
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Education_teaching")
   (DefinedFrameElementNode "#Education_teaching:Material")
)

; #Education_teaching:Precept
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Education_teaching")
   (DefinedFrameElementNode "#Education_teaching:Precept")
)

; #Education_teaching:Qualification
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Education_teaching")
   (DefinedFrameElementNode "#Education_teaching:Qualification")
)

; #Education_teaching:Role
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Education_teaching")
   (DefinedFrameElementNode "#Education_teaching:Role")
)

; #Education_teaching:Skill
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Education_teaching")
   (DefinedFrameElementNode "#Education_teaching:Skill")
)

; #Education_teaching:Student
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Education_teaching")
   (DefinedFrameElementNode "#Education_teaching:Student")
)

; #Education_teaching:Subject
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Education_teaching")
   (DefinedFrameElementNode "#Education_teaching:Subject")
)

; #Education_teaching:Teacher
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Education_teaching")
   (DefinedFrameElementNode "#Education_teaching:Teacher")
)



; Definition of Frame State

; #State:Entity
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#State")
   (DefinedFrameElementNode "#State:Entity")
)

; #State_of_entity:State
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#State")
   (DefinedFrameElementNode "#State:State")
)



; Definition of Frame State_of_entity

; #State <- #State_of_entity
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#State_of_entity")
   (DefinedFrameNode "#State")
)


; #State_of_entity:Evaluation
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#State_of_entity")
   (DefinedFrameElementNode "#State_of_entity:Evaluation")
)




; Definition of Frame Manipulation

; #Intentionally_affect <- #Manipulation
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Manipulation")
   (DefinedFrameNode "#Intentionally_affect")
)

; #Manipulation:Agent
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Manipulation")
   (DefinedFrameElementNode "#Manipulation:Agent")
)

; #Manipulation:Event
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Manipulation")
   (DefinedFrameElementNode "#Manipulation:Event")
)

; #Manipulation:Bodypart_of_agent
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Manipulation")
   (DefinedFrameElementNode "#Manipulation:Bodypart_of_agent")
)

; #Manipulation:Entity
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Manipulation")
   (DefinedFrameElementNode "#Manipulation:Entity")
)

; #Manipulation:Time
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Manipulation")
   (DefinedFrameElementNode "#Manipulation:Time")
)

; #Manipulation:Duration
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Manipulation")
   (DefinedFrameElementNode "#Manipulation:Duration")
)

; #Manipulation:Depictive
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Manipulation")
   (DefinedFrameElementNode "#Manipulation:Depictive")
)



; Definition of Frame Motion_directional

; #Motion <- #Motion_directional
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Motion_directional")
   (DefinedFrameNode "#Motion")
)


; #Motion_directional:Theme
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Motion_directional")
   (DefinedFrameElementNode "#Motion_directional:Theme")
)

; #Motion_directional:Direction
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Motion_directional")
   (DefinedFrameElementNode "#Motion_directional:Direction")
)

; #Motion_directional:Goal
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Motion_directional")
   (DefinedFrameElementNode "#Motion_directional:Goal")
)

; #Motion_directional:Path
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Motion_directional")
   (DefinedFrameElementNode "#Motion_directional:Path")
)

; #Motion_directional:Source
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Motion_directional")
   (DefinedFrameElementNode "#Motion_directional:Source")
)



; Definition of Frame Obviousness


; #Gradable_attributes <- #Obviousness
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Obviousness")
   (DefinedFrameNode "#Gradable_attributes")
)



; #Obviousness:Attribute
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Obviousness")
   (DefinedFrameElementNode "#Obviousness:Attribute")
)

; #Obviousness:Degree
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Obviousness")
   (DefinedFrameElementNode "#Obviousness:Degree")
)

; #Obviousness:Phenomenon
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Obviousness")
   (DefinedFrameElementNode "#Obviousness:Phenomenon")
)

; #Obviousness:Perceiver
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Obviousness")
   (DefinedFrameElementNode "#Obviousness:Perceiver")
)

; #Obviousness:Location_of_protagonist
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Obviousness")
   (DefinedFrameElementNode "#Obviousness:Location_of_protagonist")
)



; Definition of Frame Biological_urge

; #Gradable_attributes <- #Biological_urge
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Biological_urge")
   (DefinedFrameNode "#Gradable_attributes")
)


; #Biological_urge:Experiencer
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Biological_urge")
   (DefinedFrameElementNode "#Biological_urge:Experiencer")
)

; #Biological_urge:Expressor
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Biological_urge")
   (DefinedFrameElementNode "#Biological_urge:Expressor")
)

; #Biological_urge:State
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Biological_urge")
   (DefinedFrameElementNode "#Biological_urge:State")
)





; #Substance <- liquid
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (ConceptNode "liquid")
   (DefinedFrameNode "#Substance")
)

(ReferenceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (WordNode "#liquid")
   (ConceptNode "liquid")
)



; liquid <- water
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (ConceptNode "water")
   (ConceptNode "liquid")
)

(ReferenceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (WordNode "#water")
   (ConceptNode "water")
)



; #Food <- bone
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (ConceptNode "bone")
   (DefinedFrameNode "#Food")
)

(ReferenceLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (WordNode "#bone")
   (ConceptNode "bone")
)




; The following hierarchy representation aren't frames,
; but will be used to compose perceptions which use frames

; Object <- StaticObject
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f) 
   (ConceptNode "StaticObject")
   (ConceptNode "Object")
)

; StaticObject <- Structure
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f) 
   (ConceptNode "Structure")
   (ConceptNode "StaticObject")
)

; Object <- MovableObject
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f) 
   (ConceptNode "MovableObject")
   (ConceptNode "Object")
)

; MovableObject <- Item
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f) 
   (ConceptNode "Item")
   (ConceptNode "MovableAgent")
)

; MovableObject <- Agent
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f) 
   (ConceptNode "Agent")
   (ConceptNode "MovableAgent")
)

; Agent <- Humanoid
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f) 
   (ConceptNode "Humanoid")
   (ConceptNode "Agent")
)

; Agent <- Pet
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f) 
   (ConceptNode "Pet")
   (ConceptNode "Agent")
)

; Agent <- Avatar
(InheritanceLink (stv 1.0 1.0) (cog-new-av 0 1 #f) 
   (ConceptNode "Avatar")
   (ConceptNode "Agent")
)

; #Color:Color
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Color")
   (DefinedFrameElementNode "#Color:Color")
)
  
; #Color:Entity
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Color")
   (DefinedFrameElementNode "#Color:Entity")
)

; #Answer:Message
(FrameElementLink (stv 1.0 1.0) (cog-new-av 0 1 #f)
   (DefinedFrameNode "#Answer")
   (DefinedFrameElementNode "#Answer:Message")
)
