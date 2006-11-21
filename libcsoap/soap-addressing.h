/******************************************************************
 *  $Id: soap-addressing.h,v 1.1 2006/11/21 08:34:34 m0gg Exp $
 *
 * CSOAP Project:  A SOAP client/server library in C
 * Copyright (C) 2006 Heiko Ronsdorf
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
 * Email: hero@persua.de
 ******************************************************************/
#ifndef __csoap_addressing_h
#define __csoap_addressing_h

/**
 *
 * WS-Addressing provides transport-neutral mechanisms to address Web services
 * and messages. Specifically, this specification defines XML [XML 1.0, XML
 * Namespaces] elements to identify Web service endpoints and to secure
 * end-to-end endpoint identification in messages. This specification enables
 * messaging systems to support message transmission through networks that
 * include processing nodes such as endpoint managers, firewalls, and gateways
 * in a transport-neutral manner.
 *
 * @author	H. Ronsdorf
 * @version	$Revision: 1.1 $
 * @see		http://www.w3.org/TR/ws-addr-core/
 *
 */
#define WSA_NAMESPACE		"http://www.w3.org/2005/08/addressing"
#define WSA_NAMESPACE_PREFIX	"wsa"

/**
 *
 * Some endpoints cannot be located with a meaningful IRI; this URI is used to
 * allow such endpoints to send and receive messages. The precise meaning of
 * this URI is defined by the binding of Addressing to a specific protocol
 * and/or the context in which the EPR is used.
 *
 */
#define WSA_ANONYMOUS		WSA_NAMESPACE "/anonymous"

/**
 *
 * Messages sent to EPRs whose [address] is this value MUST be discarded (i.e.
 * not sent). This URI is typically used in EPRs that designate a reply or fault
 * endpoint (see section 3.1 Abstract Property Definitions) to indicate that no
 * reply or fault message should be sent.
 *
 */
#define WSA_NONE		WSA_NAMESPACE "/none"

/**
 *
 * Indicates that this is a reply to the message identified by the [message id]
 * IRI.
 *
 */
#define WSA_REPLY		WSA_NAMESPACE "/reply"

/**
 *
 * The wsa:EndpointReferenceType type is used wherever a Web service endpoint is
 * referenced.
 *
 */
#define WSA_ENDPOINT_REFERENCE_TYPE	"EndpointReferenceType"

/**
 *
 * This represents some element of type wsa:EndpointReferenceType. Any element
 * of type wsa:EndpointReferenceType may be used. There is an extensibility
 * mechanism to allow additional elements to be specified.
 *
 */
#define WSA_ENDPOINT_REFERENCE		"EndpointReference"

/**
 *
 * This OPTIONAL element contains the elements that convey the [reference
 * properties] of the reference. Each child element of ReferenceProperties
 * represents an individual [reference property].
 *
 */
#define WSA_REFERENCE_PROPERTIES	"ReferenceProperties"

/**
 *
 * This OPTIONAL element may contain elements from any namespace. Such elements
 * form the metadata that is relevant to the interaction with the endpoint.
 *
 */
#define WSA_METADATA			"Metadata"

/**
 *
 * This OPTIONAL element (of type xs:anyURI) conveys the [message id] property.
 * This element MUST be present if wsa:ReplyTo or wsa:FaultTo is present.
 *
 */
#define WSA_MESSAGE_ID			"MessageID"

/**
 *
 * This OPTIONAL (repeating) element information item contributes one abstract
 * [relationship] property value, in the form of a (URI, QName) pair. The
 * [children] property of this element (which is of type xs:anyURI) conveys the
 * [message id] of the related message. This element MUST be present if the
 * message is a reply.
 *
 */
#define WSA_RELATES_TO			"RelatesTo"

/**
 *
 * This OPTIONAL attribute of <wsa:RelatesTo> (of type xs:QName) conveys the
 * relationship type as a QName. When absent, the implied value of this attribute
 * is wsa:Reply.
 *
 */
#define WSA_RELATIONSHIP_TYPE		"RelationshipType"


/**
 *
 * This OPTIONAL element (of type wsa:EndpointReferenceType) provides the value
 * for the [reply endpoint] property. This element MUST be present if a reply is
 * expected. If this element is present, wsa:MessageID MUST be present.
 *
 */
#define WSA_REPLY_TO			"ReplyTo"

/**
 *
 * This OPTIONAL element (of type wsa:EndpointReferenceType) provides the value
 * for the [source endpoint] property.
 *
 */
#define WSA_FROM			"From"

/**
 *
 * This OPTIONAL element (of type wsa:EndpointReferenceType) provides the value
 * for the [fault endpoint] property. If this element is present, wsa:MessageID
 * MUST be present.
 *
 */
#define WSA_FAULT_TO			"FaultTo"

/**
 *
 * This REQUIRED element (of type xs:anyURI) provides the value for the
 * [destination] property.
 *
 */
#define WSA_TO				"To"

/**
 *
 * This REQUIRED element of type xs:anyURI conveys the [action] property. The
 * [children] of this element convey the value of this property.
 *
 */
#define WSA_ACTION			"Action"

/**
 *
 * An address URI that identifies the endpoint. This may be a network address or
 * a logical address.
 *
 */
#define WSA_ADDRESS			"Address"

#ifdef __cplusplus
extern "C" {
#endif

xmlURI *soap_addressing_get_address(xmlNodePtr endpoint_reference);
xmlNodePtr soap_addressing_set_address(xmlNodePtr endpoint_reference, xmlURI *address);

xmlNodePtr soap_addressing_get_reference_properties(xmlNodePtr endpoint_reference);
xmlNodePtr soap_addressing_set_reference_properties(xmlNodePtr endpoint_reference, xmlNodePtr property);

xmlNodePtr soap_addressing_get_metadata(xmlNodePtr endpoint_reference);
xmlNodePtr soap_addressing_set_metadata(xmlNodePtr endpoint_reference, xmlNodePtr parameter);

xmlURI *soap_addressing_get_message_id(SoapEnv *envelope);
xmlNodePtr soap_addressing_set_message_id(SoapEnv *envelope, xmlURI *id);

xmlNodePtr soap_addressing_get_relates_to(SoapEnv *envelope);
xmlNodePtr soap_addressing_add_relates_to(SoapEnv *envelope, xmlURI *id, xmlURI *type);

xmlNodePtr soap_addressing_get_reply_to(SoapEnv *envelope);
xmlNodePtr soap_addressing_set_reply_to(SoapEnv *envelope, xmlNodePtr address);

xmlURI *soap_addressing_get_reply_to_address(SoapEnv *envelope);
xmlNodePtr soap_addressing_set_reply_to_address(SoapEnv *envelope, xmlURI *address);

xmlNodePtr soap_addressing_get_from(SoapEnv *envelope);
xmlNodePtr soap_addressing_set_from(SoapEnv *envelope, xmlNodePtr address);

xmlURI *soap_addressing_get_from_address(SoapEnv *envelope);
xmlNodePtr soap_addressing_set_from_address(SoapEnv *envelope, xmlURI *address);

xmlNodePtr soap_addressing_get_fault_to(SoapEnv *envelope);
xmlNodePtr soap_addressing_set_fault_to(SoapEnv *envelope, xmlNodePtr address);

xmlURI *soap_addressing_get_fault_to_address(SoapEnv *envelope);
xmlNodePtr soap_addressing_set_fault_to_address(SoapEnv *envelope, xmlURI *address);

xmlNodePtr soap_addressing_get_to(SoapEnv *envelope);
xmlNodePtr soap_addressing_set_to(SoapEnv *envelope, xmlNodePtr address);

xmlURI *soap_addressing_get_to_address(SoapEnv *envelope);
xmlNodePtr soap_addressing_set_to_address(SoapEnv *envelope, xmlURI *address);

xmlURI *soap_addressing_get_action(SoapEnv *envelope);
xmlNodePtr soap_addressing_set_action(SoapEnv *envelope, xmlURI *action);

#ifdef __cplusplus
}
#endif

#endif
