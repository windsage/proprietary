#ifndef EVENT_H
#define EVENT_H

/*===========================================================================

                   Event Reporting Services

General Description
  All declarations and definitions necessary to support the static
  system event reporting service.

# Copyright (c) 2007-2011, 2018, 2020-2021 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/* Since the IDs and type definitions are part of the API, include it here */
#include "event_defs.h"

/* -------------------------------------------------------------------------
   Function Defintions
   ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif
/*===========================================================================

FUNCTION EVENT_REPORT

DESCRIPTION
  Report an event.

DEPENDENCIES
  Event services must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void event_report (event_id_enum_type event_id);

/*===========================================================================

FUNCTION EVENT_REPORT_PAYLOAD

DESCRIPTION
  Report an event with payload data.

DEPENDENCIES
  Event services must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void event_report_payload (event_id_enum_type event_id, uint8 length, void *data);


#ifdef __cplusplus
}
#endif /* for extern "C" modifier */
#endif         /* EVENT_H */
