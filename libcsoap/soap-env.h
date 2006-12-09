/******************************************************************
 *  $Id: soap-env.h,v 1.21 2006/12/09 09:27:11 m0gg Exp $
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
#ifndef __csoap_env_h
#define __csoap_env_h

/** @file
 *
 * \section soap_env SOAP Envelope
 *
 * A SOAP message is an XML document that consists of a mandatory SOAP envelope,
 * an optional SOAP header, and a mandatory SOAP body. This XML document is
 * referred to as a SOAP message. The namespace identifier for the elements and
 * attributes defined in this section is
 * "http://schemas.xmlsoap.org/soap/envelope/". A SOAP message contains the
 * following:
 * - The Envelope is the top element of the XML document representing the
 *   message. 
 * - The Header is a generic mechanism for adding features to a SOAP message in a
 *   decentralized manner without prior agreement between the communicating
 *   parties. SOAP defines a few attributes that can be used to indicate who
 *   should deal with a feature and whether it is optional or mandatory.
 * - The Body is a container for mandatory information intended for the ultimate
 *   recipient of the message. SOAP defines one element for the body, which is
 *   the Fault element used for reporting errors. 
 *
 * The grammar rules are as follows: 
 * - Envelope 
 *   -# The element name is "Envelope". 
 *   -# The element MUST be present in a SOAP message 
 *   -# The element MAY contain namespace declarations as well as additional
 *      attributes. If present, such additional attributes MUST be
 *      namespace-qualified. Similarly, the element MAY contain additional sub
 *      elements. If present these elements MUST be namespace-qualified and MUST
 *      follow the SOAP Body element. 
 * - Header
 *   -# The element name is "Header". 
 *   -# The element MAY be present in a SOAP message. If present, the element MUST
 *      be the first immediate child element of a SOAP Envelope element. 
 *   -# The element MAY contain a set of header entries each being an immediate
 *      child element of the SOAP Header element. All immediate child elements of
 *      the SOAP Header element MUST be namespace-qualified. 
 * - Body
 *   -# The element name is "Body". 
 *   -# The element MUST be present in a SOAP message and MUST be an immediate
 *      child element of a SOAP Envelope element. It MUST directly follow the SOAP
 *      Header element if present. Otherwise it MUST be the first immediate child
 *      element of the SOAP Envelope element. 
 *   -# The element MAY contain a set of body entries each being an immediate
 *      child element of the SOAP Body element. Immediate child elements of the
 *      SOAP Body element MAY be namespace-qualified. SOAP defines the SOAP Fault
 *      element, which is used to indicate error messages.
 *
 * \subsection soap_env_enc SOAP encodingStyle Attribute
 *
 * The SOAP encodingStyle global attribute can be used to indicate the
 * serialization rules used in a SOAP message. This attribute MAY appear on any
 * element, and is scoped to that element's contents and all child elements not
 * themselves containing such an attribute, much as an XML namespace declaration
 * is scoped. There is no default encoding defined for a SOAP message.
 *
 * The attribute value is an ordered list of one or more URIs identifying the
 * serialization rule or rules that can be used to deserialize the SOAP message
 * indicated in the order of most specific to least specific. Examples of values
 * are:
 * - "http://schemas.xmlsoap.org/soap/encoding/"
 * - "http://my.host/encoding/restricted http://my.host/encoding/"
 * - ""
 * The serialization rules defined by SOAP are identified by the URI
 * "http://schemas.xmlsoap.org/soap/encoding/". Messages using this particular
 * serialization SHOULD indicate this using the SOAP encodingStyle attribute. In
 * addition, all URIs syntactically beginning with
 * "http://schemas.xmlsoap.org/soap/encoding/" indicate conformance with the SOAP
 * encoding rules (though with potentially tighter rules added). A value of the
 * zero-length URI ("") explicitly indicates that no claims are made for the
 * encoding style of contained elements. This can be used to turn off any claims
 * from containing elements.
 *
 * \subsection soap_env_version Envelope Versioning Model
 *
 * SOAP does not define a traditional versioning model based on major and minor
 * version numbers. A SOAP message MUST have an Envelope element associated with
 * the "http://schemas.xmlsoap.org/soap/envelope/" namespace. If a message is
 * received by a SOAP application in which the SOAP Envelope element is
 * associated with a different namespace, the application MUST treat this as a
 * version error and discard the message. If the message is received through a
 * request/response protocol such as HTTP, the application MUST respond with a
 * SOAP VersionMismatch faultcode message using the SOAP
 * "http://schemas.xmlsoap.org/soap/envelope/" namespace.
 *
 */

/**
 *
 * The SOAP envelope namespace
 *
 */
static const char * const soap_env_ns = "http://schemas.xmlsoap.org/soap/envelope/";

/**
 *
 *  The SOAP envelope object. 
 *
 */
struct SoapEnv
{
  xmlNodePtr root; /** Pointer to the firts xml element (envelope) */
  xmlNodePtr header;
  xmlNodePtr body;
  xmlNodePtr cur; /** Pointer to the current xml element. (stack) */
};

typedef void (*XmlSerializerCallback) (void * obj, const xmlChar *root_element_name, void (*OnStartElement) (const xmlChar * element_name, int attr_count, xmlChar ** keys, xmlChar ** values, void *userData), void (*OnCharacters) (const xmlChar * element_name, const xmlChar * chars, void *userData), void (*OnEndElement) (const xmlChar * element_name, void *userData), void *userdata);

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Creates an envelope with a fault object. 
 *
 * @param faultcode The fault code @see fault_code_t
 * @param faultstring A fault message
 * @param faultactor The fault actor (This can be NULL)
 * @param detail The detail of the error (This can be NULL)
 * @param out the result envelope out parameter like follows
 *
 *  <pre>
 *  <SOAP-ENV:Envelope xmlns:SOAP-ENV="..." SOAP-ENV:encoding="..."
 *       xmlns:xsi="..."
 *     xmlns:xsd="...">
 *   <SOAP-ENV:Body>
 *
 *	 <Fault>
 *	  <faultcode>...</faultcode>
 *	  <faultstring>...</faultstring>
 *	  <faultactor>...</faultactor>
 *	  <faultdetail>..</faultdetail>
 *	 </Fault>
 *
 *   </SOAP-ENV:Body>
 *  </SOAP-ENV:Envelope>
 *  </pre>
 *
 * @returns H_OK if success
 *
 * @see	soap_fault_new
 *
 */
extern herror_t soap_env_new_with_fault(int faultCode, const char *faultString, const char *faultActor, const char *detail, struct SoapEnv **out);

/**
 *
 * Creates an envelope with a method to invoke a soap service.
 * Use this function to create a client call.
 *
 * @param urn The urn of the soap service to invoke
 * @param method The method name of the soap service
 *
 * @param out the result envelope out parameter like follows
 * @returns H_OK if success
 *
 * <pre>
 * <SOAP-ENV:Envelope xmlns:SOAP-ENV="..." SOAP-ENV:encoding="..."
 *      xmlns:xsi="..."
 *      xmlns:xsd="...">
 *    <SOAP-ENV:Body>
 *
 *     <m:[method] xmlns:m="[urn]">
 *     </m:[method]>
 *
 *    </SOAP-ENV:Body>
 * </SOAP-ENV:Envelope>
 * 
 * </pre>
 *
 */
extern herror_t soap_env_new_with_method(const char *urn, const char *method, struct SoapEnv ** out);

/**
 *
 * Creates a soap envelope with a response. 
 * Use this function to create a response envelope object
 * for a request. This function is only relevant for soap 
 * service implementors. 
 *
 * @see example csoap/simpleserver.c
 *
 * @param req The request object. A response object will be created
 *  to this request.
 *
 * @param out the result envelope out paramter like follows
 * @returns H_OK if success
 *
 * <pre>
 * <SOAP-ENV:Envelope xmlns:SOAP-ENV="..." SOAP-ENV:encoding="..."
 *      xmlns:xsi="..."
 *      xmlns:xsd="...">
 *    <SOAP-ENV:Body>
 *
 *     <m:[req-method]Response xmlns:m="[req-urn]">
 *     </m:[req-method]>
 *
 *    </SOAP-ENV:Body>
 * </SOAP-ENV:Envelope>
 * 
 * </pre>
 *
 *
 */
extern herror_t soap_env_new_with_response(struct SoapEnv * req, struct SoapEnv ** out);

/**
 *
 * Creates an envelope from a given libxml2 xmlDocPtr.
 *
 * @param doc the xml document pointer
 * @param out the output envelope object
 *
 * @returns H_OK if success
 *  
 */
extern herror_t soap_env_new_from_doc(xmlDocPtr doc, struct SoapEnv ** out);

/**
 *
 * Create an envelop object from a string. 
 * The string must be in xml format.
 *
 * @param buffer The string to parse into a envelope.
 * @param out the output envelope object
 * @returns H_OK if success
 */
extern herror_t soap_env_new_from_buffer(const char *buffer, struct SoapEnv ** out);

/**
 *
 * Adds a new xml node under the current parent.
 *
 * <pre>
 *  <m:[name] type=[type]>[value]</m:[name]>
 * </pre>
 * 
 * @param env The envelope object 
 * @param type Type of the parameter. Something like "xsd:string" or
 *  "xsd:int" or custom types. 
 * @param name Name of the xml node
 * @param value Text value of the xml node
 *
 * @returns The added xmlNode pointer.
 * 
 * @see tutorial 
 *
 */
extern xmlNodePtr soap_env_add_item(struct SoapEnv * env, const char *type, const char *name, const char *value);

/**
 *
 *  Adds attachment href node to the envelope current parent.
 *
 * <pre>
 *  <m:[name] href=[href]/>
 * </pre>
 *
 * @param env The envelope object 
 * @param name Name of the xml node
 * @param href href. A CID string filled by 
 *   soap_ctx_add_attachment()
 *
 * @returns The added xmlNode pointer.
 * 
 * @see soap_ctx_add_file tutorial 
 */
extern xmlNodePtr
soap_env_add_attachment(struct SoapEnv * env, const char *name, const char *href);

/**
 *
 *  Serialize and adds obj to the envelope.
 *  TODO: Document this function !
 *  <br>
 *  <b>Important: </b> 
 *
 */
extern void soap_env_add_custom(struct SoapEnv * env, void *obj, XmlSerializerCallback cb, const char *type, const char *name);

/**
 *
 * Same as soap_env_add_item() with c style arguments
 * like in printf(). "value" is the format string.
 * <br>
 * <b>Important: </b> The totally length of value (incl. args)
 *  must be lower the 1054.
 *
 * @see soap_env_add_item
 */
extern xmlNodePtr
soap_env_add_itemf(struct SoapEnv * env, const char *type, const char *name, const char *value, ...);

/**
 *
 * Push the current xml node in the soap envelope one level 
 * deeper. Here an example:
 * 
 * <pre>
 * soap_env_push_item(env, "my:custom", "Person");
 *  soap_env_add_item(env, "xsd:string", "name", "Mickey");
 *  soap_env_add_item(env, "xsd:string", "lastname", "Mouse");
 * soap_env_pop_item(env);
 * </pre>
 *
 * This will create the xml like follows.
 *
 * <pre>
 * <Person type="my:custom">
 *  <name>Mickey</name>
 *  <lastname>Mouse</lastname>
 * </Person>
 * </pre>
 *
 * @returns The added xmlNode pointer.
 * 
 * @see tutorial 
 *
 */
extern xmlNodePtr soap_env_push_item(struct SoapEnv * env, const char *type, const char *name);

/**
 *
 *  Sets the xml pointer 1 level higher. 
 *
 *  @param env The envelope object
 *
 *  @see soap_env_push_item
 */
extern void soap_env_pop_item(struct SoapEnv * env);

/**
 *
 *  Free the envelope. 
 *
 *  @param env The envelope object
 *
 */
extern void soap_env_free(struct SoapEnv *env);

/**
 *
 * Gets the xml node pointing to SOAP Body.
 *
 */
extern xmlNodePtr soap_env_get_body(struct SoapEnv * env);

/**
 *
 * Get the xml node pointing to SOAP method (call)
 *
 */
extern xmlNodePtr soap_env_get_method(struct SoapEnv * env);

/**
 *
 *  Get the xml node pointing to SOAP Fault
 *
 */
extern xmlNodePtr soap_env_get_fault(struct SoapEnv * env);

/**
 *
 * Get the xml node pointing to SOAP Header
 *
 */
extern xmlNodePtr soap_env_get_header(struct SoapEnv * env);

/**
 *
 * Get the URN of the message.
 *
 */
extern const char *soap_env_find_urn(struct SoapEnv * env);

/**
 *
 * Get the method of the message.
 *
 */
extern const char *soap_env_find_methodname(struct SoapEnv * env);

#ifdef __cplusplus
}
#endif

#endif
