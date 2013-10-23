from spatiotemporal.temporal_events import TemporalEventTrapezium
from spatiotemporal.unix_time import UnixTime

__author__ = 'keyvan'

from temporal_formulas import *

TEMPORAL_RELATIONS = {
    'p': 'precedes',
    'm': 'meets',
    'o': 'overlaps',
    'F': 'finished by',
    'D': 'contains',
    's': 'starts',
    'e': 'equals',
    'S': 'started by',
    'd': 'during',
    'f': 'finishes',
    'O': 'overlapped by',
    'M': 'met by',
    'P': 'preceded by'
}


def temporal_relation_between(temporal_event_1, temporal_event_2):
    temporal_event_1 = TemporalEventTrapezium(1, 10)
    temporal_event_2 = TemporalEventTrapezium(11, 16)

    sum_times = sorted(
        [UnixTime(time).to_datetime() for time in set(temporal_event_1.to_list() + temporal_event_2.to_list())]
    )
    sum_certainties = []
    for time_step in sum_times:
        sum_certainties.append(temporal_event_1.membership_function(time_step) +
                               temporal_event_2.membership_function(time_step))

    result = {}

    formulas = {
        'p': beforeFormula,
        'm': meetsFormula,
        'o': overlapsFormula,
        'F': finished_byFormula,
        'D': containsFormula,
        's': startsFormula,
        'e': equalsFormula,
        'S': started_byFormula,
        'd': duringFormula,
        'f': finishesFormula,
        'O': overlapped_byFormula,
        'M': met_byFormula,
        'P': afterFormula
    }

    dist1, dist2 = temporal_event_1.to_dict(), temporal_event_2.to_dict()
    for name in formulas:
        degree = formulas[name](dist1, dist2)
        if degree > 0:
            result[name] = degree

    print result
    temporal_event_1.plot()
    plt = temporal_event_2.plot()
    plt.plot(sum_times, sum_certainties)
    plt.show()

    return result


def create_event_relation_hashtable(temporal_events):
    table = {}

    for A in temporal_events:
        for B in temporal_events:
            if B == A:
                continue
            for C in temporal_events:
                if C in (A, B):
                    continue
                for r1 in TEMPORAL_RELATIONS:
                    for r2 in TEMPORAL_RELATIONS:
                        for r3 in TEMPORAL_RELATIONS:
                            relation1 = TemporalRelation(r1, A, B)
                            relation2 = TemporalRelation(r2, B, C)
                            relation3 = TemporalRelation(r3, A, C)
                            degrees = (relation1.degree(), relation2.degree(), relation3.degree())
                            if degrees != (0, 0, 0):
                                table[(r1, r2, r3)] = degrees

    return table


if __name__ == '__main__':
    #from spatiotemporal.temporal_events import generate_random_events, BaseTemporalEvent, TemporalEventLinearPiecewise

    #events = generate_random_events(10)
    #table = create_event_relation_hashtable(events)
    #print table

    print temporal_relation_between(1, 2)
