/******************************************************************
 *  $Id: soap-fault.h,v 1.6 2006/11/21 20:59:02 m0gg Exp $
 *
 * CSOAP Project:  A SOAP client/server library in C
 * Copyright (C) 2003  Ferhat Ayaz
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 * 
 * Email: ayaz@jprogrammer.net
 ******************************************************************/
#ifndef __csoap_fault_h
#define __csoap_fault_h

/**
 *
 * The SOAP Fault element is used to carry error and/or status information within
 * a SOAP message. If present, the SOAP Fault element MUST appear as a body entry
 * and MUST NOT appear more than once within a Body element.
 *
 * The SOAP Fault element defines the following four subelements:
 *
 * # faultcode - The faultcode element is intended for use by software to provide
 *   an algorithmic mechanism for identifying the fault. The faultcode MUST be
 *   present in a SOAP Fault element and the faultcode value MUST be a qualified
 *   name as defined in "Namespaces in XML", section 3. SOAP defines a small set
 *   of SOAP fault codes covering basic SOAP faults (see section 4.4.1).
 * # faultstring - The faultstring element is intended to provide a human
 *   readable explanation of the fault and is not intended for algorithmic
 *   processing. The faultstring element is similar to the 'Reason-Phrase' defined
 *   by HTTP (see RFC2616, section 6.1). It MUST be present in a SOAP Fault
 *   element and SHOULD provide at least some information explaining the nature
 *   of the fault.
 * # faultactor - The faultactor element is intended to provide information about
 *   who caused the fault to happen within the message path (see section 2). It is
 *   similar to the SOAP actor attribute (see section 4.2.2) but instead of
 *   indicating the destination of the header entry, it indicates the source of
 *   the fault. The value of the faultactor attribute is a URI identifying the
 *   source. Applications that do not act as the ultimate destination of the SOAP
 *   message MUST include the faultactor element in a SOAP Fault element. The
 *   ultimate destination of a message MAY use the faultactor element to indicate
 *   explicitly that it generated the fault (see also the detail element below).
 * # detail - The detail element is intended for carrying application specific
 *   error information related to the Body element. It MUST be present if the
 *   contents of the Body element could not be successfully processed. It MUST
 *   NOT be used to carry information about error information belonging to header
 *   entries. Detailed error information belonging to header entries MUST be
 *   carried within header entries.
 *
 */
 
/**
 *
 * The faulting node found an invalid element information item instead of the
 * expected Envelope element information item. The namespace, local name or both
 * did not match the Envelope element information item required by this
 * recommendation (see 2.8 SOAP Versioning Model and 5.4.7 VersionMismatch
 * Faults)
 *
 */
#define SOAP_FAULT_VERSION_MISMATCH	0

/**
 *
 * An immediate child element information item of the SOAP Header element
 * information item targeted at the faulting node that was not understood by the
 * faulting node contained a SOAP mustUnderstand attribute information item with
 * a value of "true" (see 5.2.3 SOAP mustUnderstand Attribute and 5.4.8 SOAP
 * mustUnderstand Faults)
 *
 */
#define SOAP_FAULT_MUST_UNDERSTAND	1

/**
 *
 * A SOAP header block or SOAP body child element information item targeted at
 * the faulting SOAP node is scoped (see 5.1.1 SOAP encodingStyle Attribute) with
 * a data encoding that the faulting node does not support.
 *
 */
#define SOAP_FAULT_DATA_ENCODING_UNKOWN	2

/**
 *
 * The message could not be processed for reasons attributable to the processing
 * of the message rather than to the contents of the message itself. For example,
 * processing could include communicating with an upstream SOAP node, which did
 * not respond. The message could succeed if resent at a later point in time (see
 * also 5.4 SOAP Fault for a description of the SOAP fault detail sub-element).
 *
 */
#define SOAP_FAULT_RECEIVER		4

/**
 *
 * The message was incorrectly formed or did not contain the appropriate
 * information in order to succeed. For example, the message could lack the
 * proper authentication or payment information. It is generally an indication
 * that the message is not to be resent without change (see also 5.4 SOAP Fault
 * for a description of the SOAP fault detail sub-element).
 *
 */
#define SOAP_FAULT_SENDER		3

#ifdef __cplusplus
extern "C" {
#endif

extern xmlDocPtr soap_fault_build(int fault_code, const char *fault_string, const char *fault_actor, const char *detail);

#ifdef __cplusplus
}
#endif

#endif
