from unittest import TestCase

from opencog_helper import find_subclasses
import opencog
import test_agent

class HelperTest(TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_find_agents(self):
        x=find_subclasses(test_agent,opencog.MindAgent)
        self.assertEqual(len(x),1)
        self.assertEqual(x[0][0], 'TestAgent')

