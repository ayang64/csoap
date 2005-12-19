/*
 * Copyright (C) 2001-2005  Rochester Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 * Author:		Matt Campbell
 * Descrip:		SSL connection routines
 */

#include <sys/types.h>
#include <netinet/in.h>
#if HAVE_INTTYPES_H
#include <inttypes.h>
#else
# if HAVE_STDINT_H
# include <stdint.h>
# else
typedef unsigned int uint32_t;
# endif
#endif

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <fcntl.h>

#include "nanohttp-ssl.h"
#include "nanohttp-common.h"


#ifdef WIN32
#include <io.h>
typedef unsigned int uint32_t;
#else
#include <unistd.h>
#endif

#define MAXCHUNK 1024
#define HEADER_LEN 5
char HEADER[HEADER_LEN] = {186, 84, 202, 86, 224};
static char *pass;

/*
 * superseed
 * Creates a 1k random seed and uses it to seed
 * the SSL random number generator
 */

void superseed()
{
	int buf[256], i;

	srand(time(NULL));

	for(i = 0; i < 256; i++) {
		buf[i] = rand();
	} RAND_seed( (unsigned char*)buf, sizeof(buf));
}

static int pw_cb(char* buf, int num, int rwflag, void *userdata)
{
	if( num < (int)strlen(pass) + 1)
		return(0);

	strcpy(buf, pass);
	return strlen(pass);
}

int verify_sn(X509* cert, int who, int nid, char* str)
{
	char	name[256];
	char	buf[256];

	memset(name, '\0', 256);
	memset(buf, '\0', 256);

	if(who == CERT_SUBJECT) {
		X509_NAME_oneline(X509_get_subject_name(cert), name, 256);
	} else {
		X509_NAME_oneline(X509_get_issuer_name(cert), name, 256);
	}

	buf[0] = '/';
	strcat(buf, OBJ_nid2sn(nid));
	strcat(buf, "=");
	strcat(buf, str);

	if(strstr(name, buf)) {
		return 1;
	} else {
		return 0;
	}
}

static int verify_cb(int prev_ok, X509_STORE_CTX* ctx)
{
	X509* cert = X509_STORE_CTX_get_current_cert(ctx);
	int depth = X509_STORE_CTX_get_error_depth(ctx);
    int err = X509_STORE_CTX_get_error(ctx);

    if( err = X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN ){
        return 1;
    }
	if(depth == 0) {
		return user_verify(cert);
	} else {
		return prev_ok;
	}
}

int user_verify(X509* cert)
{   
    //TODO: Make sure that the client is providing a client cert,
    //or that the Module is providing the Module cert
    /* connect to anyone */
    log_verbose1("Validating certificate.");
    return 1;
} 

SSL_CTX *initialize_ctx(char* keyfile, char* password, char* calist)
{
	SSL_CTX* ctx = NULL;

    if(password == NULL) password = "";


	/* Global system initialization */
	log_verbose1( "Initializing library");
	SSL_library_init();
	SSL_load_error_strings();
	ERR_load_crypto_strings();
	OpenSSL_add_ssl_algorithms();

	/* Create our context*/
    ctx = SSL_CTX_new( SSLv23_method() );

	if(ctx == NULL) {
		log_error1( "Cannot create SSL context");
		return NULL;
	}
	log_verbose1( "SSL context created ok");

	/* Load our keys and certificates*/
	if(keyfile != NULL && password != NULL ){

        if(!( SSL_CTX_use_certificate_file(ctx, keyfile, SSL_FILETYPE_PEM))){
            log_error2( "Couldn't read certificate file: %s", keyfile);
            SSL_CTX_free(ctx);
            return ctx = NULL;
        }

        log_verbose1("Certificate file read ok");

        pass = password;
        SSL_CTX_set_default_passwd_cb(ctx, pw_cb);

        if( !( SSL_CTX_use_PrivateKey_file(ctx, keyfile, SSL_FILETYPE_PEM) ) ) {
            log_error2( "Couldn't read key file: %s", keyfile);
            SSL_CTX_free(ctx);
            return ctx = NULL;
        }

        log_verbose1("Keyfile read ok");
    }
    if( calist != NULL) {

        /* Load the CAs we trust */
        if( !( SSL_CTX_load_verify_locations(ctx, calist, NULL) ) ) {
            log_error2("Couldn't read CA list: %s", calist);
            SSL_CTX_free(ctx);
            return ctx = NULL;
        }

        SSL_CTX_set_client_CA_list( ctx, SSL_load_client_CA_file(calist) );
	    log_verbose1("Certificate Authority contacted");

    }
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE,
			verify_cb);
    log_verbose1("Verify callback registered");

    SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_OFF );


	/* Load randomness */
	superseed();

	return ctx;
}

void log_ssl_error(SSL* ssl, int ret)
{
	int errqueue;
	char errorbuf[256] = "Error: ";

	if(ret == 0) {
		log_error1("SSL handshake was not successful, contolled shutdown");
	} else if(ret == -1) {
		log_error1("SSL handshake was not successful, fatal error at protocol");
	}

	errqueue = SSL_get_error(ssl, ret);

	switch(errqueue) {
		case SSL_ERROR_NONE:
			strcat(errorbuf, "None");
			break;
		case SSL_ERROR_ZERO_RETURN:
			strcat(errorbuf, "Zero return");
			break;
		case SSL_ERROR_WANT_READ:
			strcat(errorbuf, "Want read");
			break;
		case SSL_ERROR_WANT_WRITE:
			strcat(errorbuf, "Want write");
			break;
		case SSL_ERROR_WANT_X509_LOOKUP:
			strcat(errorbuf, "Want x509 lookup");
			break;
		case SSL_ERROR_SYSCALL:
			strcat(errorbuf, "Syscall:");
			if(ret == 0) {
				strcat(errorbuf, "Protocol violation");
			} else if(ret == -1) {
				strcat(errorbuf, "BIO reported an I/O error");
			} else {
				strcat(errorbuf, "Unknown syscall error");
			} /* if */

			break;
		case SSL_ERROR_SSL:
			strcat(errorbuf, "SSL library");
            while(errqueue=ERR_get_error()){
                log_error2("SSL %s", ERR_error_string(errqueue, NULL));
            }
			break;
	} /* switch code */

	log_error1( errorbuf);
}

SSL* init_ssl(SSL_CTX* ctx, int sock, int type)
{
	int ret;
    int status;
	SSL* ssl;
#if 0
#ifdef WIN32
	BIO* rbio;
	BIO* wbio;
#else
	BIO* sbio;
#endif
#endif

    log_verbose1("Starting SSL Initialization");
    
	ssl = SSL_new(ctx);
      
	if(ssl == NULL) {
		log_error1( "Cannot create new ssl object");
		return NULL;
	}

#if 0
#ifdef WIN32
    log_error1("Setting up BIO with socket");
    rbio = BIO_new_socket(sock, BIO_NOCLOSE);
    if( rbio == NULL ) {
        log_error1( "BIO_new_socket failed");
        return NULL;
    }
    SSL_set_bio(ssl, rbio, rbio);

#else
	sbio = BIO_new_socket(sock, BIO_NOCLOSE);

	if( sbio == NULL ) {
		log_error1( "BIO_new_socket failed");
		return NULL;
	}
	SSL_set_bio(ssl, sbio, sbio);
#endif
#endif
    SSL_set_fd(ssl, sock);

	if(type == SSL_SERVER) {
		ret = SSL_accept(ssl);
		if(ret <= 0) {
			log_error1( "SSL accept error");
			log_ssl_error(ssl, ret);
			SSL_free(ssl);
			return ssl = NULL;
		} /* if error */
	} else { /* client */
		ret = SSL_connect(ssl);
		if(ret <= 0) {
			log_error1( "SSL connect error");
			log_ssl_error(ssl, ret);
			SSL_free(ssl);
			return ssl = NULL;
		} /* if error */
        /* SSL_connect should take care of this for us.
        if(SSL_get_peer_certificate(ssl) == NULL) {
            log_error1( "No certificate provided");
            SSL_free(ssl);
            return ssl = NULL;
        }
        if(SSL_get_verify_result(ssl) != X509_V_OK) {
            log_error1( "Certificate did not verify");
            SSL_free(ssl);
            return ssl = NULL;
        }
        */
	}
	
    log_verbose1("Completed SSL Initialization");
	return ssl;
}

void ssl_cleanup(SSL* ssl)
{
	/* does nothing to context */

	if(ssl != NULL) {

		SSL_shutdown(ssl);
//        SSL_clear(ssl);
		SSL_free(ssl);
		ssl = NULL;
	}
}

