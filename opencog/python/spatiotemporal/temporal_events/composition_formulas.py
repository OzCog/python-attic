from spatiotemporal.temporal_events import TemporalEventTrapezium
from spatiotemporal.unix_time import UnixTime

__author__ = 'keyvan'


def create_event_relation_hashtable(temporal_events):
    table = {}

    #for A in temporal_events:
    #    for B in temporal_events:
    #        if B == A:
    #            continue
    #        for C in temporal_events:
    #            if C in (A, B):
    #                continue
    #            for r1 in TEMPORAL_RELATIONS:
    #                for r2 in TEMPORAL_RELATIONS:
    #                    for r3 in TEMPORAL_RELATIONS:
    #                        relation1 = TemporalRelation(r1, A, B)
    #                        relation2 = TemporalRelation(r2, B, C)
    #                        relation3 = TemporalRelation(r3, A, C)
    #                        degrees = (relation1.degree(), relation2.degree(), relation3.degree())
    #                        if degrees != (0, 0, 0):
    #                            table[(r1, r2, r3)] = degrees

    return table


if __name__ == '__main__':
    #from spatiotemporal.temporal_events import generate_random_events, BaseTemporalEvent, TemporalEventLinearPiecewise

    #events = generate_random_events(10)
    #table = create_event_relation_hashtable(events)
    #print table

    print temporal_relation_between(1, 2)
