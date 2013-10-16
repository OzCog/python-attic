from datetime import datetime
from scipy import integrate
from spatiotemporal.interval import Interval, assert_is_interval
from spatiotemporal.membership_function import FuzzyMembershipFunction

__author__ = 'keyvan'


class BaseTemporalEvent(Interval):
    _membership_function = None

    def __init__(self, a, b, iter_step=1):
        Interval.__init__(self, a, b, iter_step=iter_step)
        self._membership_function = FuzzyMembershipFunction(self, self.membership_function_single_point)

    def membership_function(self, time=None):
        return self._membership_function(time)

    def membership_function_single_point(self, time_step):
        """
        to override, membership_function calls to it
        alternatively one can directly override membership_function
        """
        return 0

    def _interval_from_self_if_none(self, a, b, interval):
        if interval is None:
            if (a, b) == (None, None):
                interval = self
            else:
                interval = Interval(a, b)
        else:
            assert_is_interval(interval)
        return interval

    def degree_in_interval(self, a=None, b=None, interval=None):
        """
        use either 'a' and 'b' or 'interval'
        """
        interval = self._interval_from_self_if_none(a, b, interval)
        return integrate.quad(self.membership_function, interval.a, interval.b) / interval.duration

    def __repr__(self):
        return '{0}(a:{1}, b:{2})'.format(self.__class__.__name__, self.a, self.b)

    def __str__(self):
        return repr(self)


class TemporalEventSimple(BaseTemporalEvent):
    def membership_function_single_point(self, time_step):
        if self.a <= time_step <= self.b:
            return 1
        return 0


class TemporalEventDistributional(BaseTemporalEvent):
    def __init__(self, a, b, pdf, iter_step=1):
        assert callable(pdf), "'pdf' should be callable"
        Interval.__init__(self, a, b, iter_step=iter_step)
        self.membership_function = pdf
        self.membership_function_single_point = pdf

if __name__ == '__main__':
    import matplotlib.pyplot as plt

    event = TemporalEventSimple(datetime(2010, 1, 1), datetime(2011, 2, 1), iter_step=100)
    event = plt.plot(event.to_list(), event.membership_function())
    plt.show()