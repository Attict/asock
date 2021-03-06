/* This is a basic TCP/TLS echo server. */

#include "../src/asock.h"
const int SSL = 1;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Our socket extension */
struct echo_socket
{
	char *backpressure;
	int length;
};

/* Our socket context extension */
struct echo_context
{

};

/* Loop wakeup handler */
void on_wakeup(asock_loop_t *loop)
{

}

/* Loop pre iteration handler */
void on_pre(asock_loop_t *loop)
{

}

/* Loop post iteration handler */
void on_post(asock_loop_t *loop)
{

}

/* Socket writable handler */
asock_socket_t *on_echo_socket_writable(asock_socket_t *s)
{
	struct echo_socket *es = (struct echo_socket *) asock_socket_ext(SSL, s);

	/* Continue writing out our backpressure */
	int written = asock_socket_write(SSL, s, es->backpressure, es->length, 0);
	if (written != es->length) {
		char *new_buffer = (char *) malloc(es->length - written);
		memcpy(new_buffer, es->backpressure, es->length - written);
		free(es->backpressure);
		es->backpressure = new_buffer;
		es->length -= written;
	} else {
		free(es->backpressure);
		es->length = 0;
	}

	/* Client is not boring */
	asock_socket_timeout(SSL, s, 30);

	return s;
}

/* Socket closed handler */
asock_socket_t *on_echo_socket_close(asock_socket_t *s) {
	struct echo_socket *es = (struct echo_socket *) asock_socket_ext(SSL, s);

	printf("Client disconnected\n");

	free(es->backpressure);

	return s;
}

/* Socket half-closed handler */
asock_socket_t *on_echo_socket_end(asock_socket_t *s) {
	asock_socket_shutdown(SSL, s);
	return asock_socket_close(SSL, s);
}

/* Socket data handler */
asock_socket_t *on_echo_socket_data(asock_socket_t *s, char *data, int length)
{
	struct echo_socket *es = (struct echo_socket *) asock_socket_ext(SSL, s);

	/* Print the data we received */
	printf("Client sent <%.*s>\n", length, data);

	/* Send it back or buffer it up */
	int written = asock_socket_write(SSL, s, data, length, 0);
	if (written != length)
    {
		char *new_buffer = (char *) malloc(es->length + length - written);
		memcpy(new_buffer, es->backpressure, es->length);
		memcpy(new_buffer + es->length, data + written, length - written);
		free(es->backpressure);
		es->backpressure = new_buffer;
		es->length += length - written;
	}

	/* Client is not boring */
	asock_socket_timeout(SSL, s, 30);

	return s;
}

/* Socket opened handler */
asock_socket_t *on_echo_socket_open(asock_socket_t *s, int is_client,
    char *ip, int ip_length)
{
	struct echo_socket *es = (struct echo_socket *) asock_socket_ext(SSL, s);

	/* Initialize the new socket's extension */
	es->backpressure = 0;
	es->length = 0;

	/* Start a timeout to close the socekt if boring */
	asock_socket_timeout(SSL, s, 30);

	printf("Client connected\n");

	return s;
}

/* Socket timeout handler */
asock_socket_t *on_echo_socket_timeout(asock_socket_t *s)
{
	printf("Client was idle for too long\n");
	return asock_socket_close(SSL, s);
}

int main()
{
	/* The event loop */
	asock_loop_t *loop = asock_loop_create(0, on_wakeup, on_pre, on_post, 0);

	/* Socket context */
	asock_options_t options = {};
	options.key_file_name = "misc/cert.key";
	options.cert_file_name = "misc/cert.crt";
	//options.passphrase = "1234";

	asock_context_t *echo_context = asock_context_create(SSL, loop,
      sizeof(struct echo_context), options);

	/* Registering event handlers */
	asock_context_on_open(SSL, echo_context, on_echo_socket_open);
	asock_context_on_data(SSL, echo_context, on_echo_socket_data);
	asock_context_on_writable(SSL, echo_context, on_echo_socket_writable);
	asock_context_on_close(SSL, echo_context, on_echo_socket_close);
	asock_context_on_timeout(SSL, echo_context, on_echo_socket_timeout);
	asock_context_on_end(SSL, echo_context, on_echo_socket_end);

	/* Start accepting echo sockets */
	asock_core_listen_t *listen_socket = asock_context_listen(SSL, echo_context, 0, 3000, 0, sizeof(struct echo_socket));

	if (listen_socket) {
		printf("Listening on port 3000...\n");
		asock_loop_run(loop);
	} else {
		printf("Failed to listen!\n");
	}
}
