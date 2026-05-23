/**
 * \file traff.h
 * \brief Traff related functions
 * \author Olaf Brandt <olf@eisenzelt.de>
 *
 *   This is a library to create TraFF meassges from TMC messages
 */

#ifndef TRAFF_H
#define TRAFF_H

#include "rds.h"
#include "stdbool.h"

// Replaces ...
typedef struct {
    char* IdAttr; // attr
} Replaces;

// Supplementary_info ...
typedef struct {
    char* ClassAttr; // attr
    char* Q_dimensionAttr; // attr, optional
    char* Q_durationAttr; // attr, optional
    char* Q_intAttr; // attr, optional
    char* Q_intsAttr; // attr, optional
    char* Q_speedAttr; // attr, optional
    char* Q_temperatureAttr; // attr, optional
    char* Q_timeAttr; // attr, optional
    char* Q_weightAttr; // attr, optional
    char* TypeAttr; // attr
} Supplementary_info;

// Event ...
typedef struct {
    char* ClassAttr; // attr
    int LengthAttr; // attr, optional
    int ProbabilityAttr; // attr, optional
    char* Q_dimensionAttr; // attr, optional
    char* Q_durationAttr; // attr, optional
    char* Q_intAttr; // attr, optional
    char* Q_intsAttr; // attr, optional
    char* Q_speedAttr; // attr, optional
    char* Q_temperatureAttr; // attr, optional
    char* Q_timeAttr; // attr, optional
    char* Q_weightAttr; // attr, optional
    int SpeedAttr; // attr, optional
    char* TypeAttr; // attr
    Supplementary_info Supplementary_info;
} Event;

// From ...
typedef struct {
    float DistanceAttr; // attr, optional
    char* Junction_nameAttr; // attr, optional
    char* Junction_refAttr; // attr, optional
} From;

// To ...
typedef struct {
    float DistanceAttr; // attr, optional
    char* Junction_nameAttr; // attr, optional
    char* Junction_refAttr; // attr, optional
} To;

// At ...
typedef struct {
    float DistanceAttr; // attr, optional
    char* Junction_nameAttr; // attr, optional
    char* Junction_refAttr; // attr, optional
    char* RadiusAttr; // attr, optional
} At;

// Via ...
typedef struct {
    float DistanceAttr; // attr, optional
    char* Junction_nameAttr; // attr, optional
    char* Junction_refAttr; // attr, optional
    char* RadiusAttr; // attr, optional
} Via;

// Not_via ...
typedef struct {
    float DistanceAttr; // attr, optional
    char* Junction_nameAttr; // attr, optional
    char* Junction_refAttr; // attr, optional
    char* RadiusAttr; // attr, optional
} Not_via;

typedef char* Polyline;

// Merge ...
typedef struct {
    Replaces Replaces;
} Merge;

// Events ...
typedef struct {
    int count;
    Event Event[];
} Events;

// Location ...
typedef struct {
    char* CountryAttr; // attr, optional
    char* DestinationAttr; // attr, optional
    char* DirectionAttr; // attr, optional
    char* DirectionalityAttr; // attr, optional
    char* FuzzinessAttr; // attr, optional
    char* OriginAttr; // attr, optional
    char* RampsAttr; // attr, optional
    char* Road_classAttr; // attr, optional
    bool Road_is_urbanAttr; // attr, optional
    char* Road_nameAttr; // attr, optional
    char* Road_refAttr; // attr, optional
    char* TerritoryAttr; // attr, optional
    char* TownAttr; // attr, optional
    From From;
    To To;
    At At;
    Via Via;
    Not_via Not_via;
    char* Polyline;
} Location;

// Message ...
typedef struct {
    bool CancellationAttr; // attr, optional
    char* End_timeAttr; // attr, optional
    char* Expiration_timeAttr; // attr, optional
    bool ForecastAttr; // attr, optional
    char* IdAttr; // attr
    char* Receive_timeAttr; // attr
    char* Start_timeAttr; // attr, optional
    char* Update_timeAttr; // attr
    char* UrgencyAttr; // attr, optional
    Merge Merge;
    Location Location;
    Events Events;

} Message;

// Fuzziness ...
typedef char* Fuzziness;

// FeedType ...
typedef struct {
    int count;
    void* Message;
} FeedType;

typedef FeedType Feed;

// Response ...
typedef struct {
    FeedType Feed;
} Response;

void traff_create_from_tmc_message(rds_oda_tmc_message_t *msg);
void remove_traff_message(rds_oda_tmc_message_t *msg);
void find_traff_message(rds_oda_tmc_message_t *msg, Message** traff_msg);

#endif
