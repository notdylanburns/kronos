#include <stdio.h>
#include <stdlib.h>

#include "../inc/kronos.h"

/*  
    The following lines declare the get_root function using the ROUTE macro 
    found in the server-utils.h header. The argument serves as the name of the
    function, which will be used to handle requests to a web route. The function
    has two arguments passed to it, 'req' and 'res', which are pointers to
    'HTTPRequest' structs and 'HTTPResponse' structs respectively.
*/
ROUTE(get_root) {
    // This function will serve static http content.
    // (The macro STATIC was created for the same purpose and is shown off below)

    /*
        The set_status() function takes four arguments:
            1) A pointer to the HTTPResponse struct on which to act
            2) The HTTP version (only 1.1 is supported for now)
            3) The status code itself (http.h lists all status codes in an enum)
            4) A string message to send with the status code
    */
    set_status(res, "HTTP/1.1", STATUS_OK, "OK");

    FILE *f = fopen("example1.html", "rb"); // Open our html file
    
    if (f == NULL) {
        /*
            If the file cannot be opened we return a server error code to
            let the browser know it's a server side issue.
        */
        set_status(res, "HTTP/1.1", STATUS_INTERNAL_SERVER_ERROR, "Internal Server Error"); // Set 500 status code
        return;
    }

    // The following two lines get the file length in bytes
    fseek(f, 0, SEEK_END);
    int length = ftell(f);

    // Reset the file pointer to the start of the file so the read is not offset
    rewind(f);

    // Create a character buffer the same length as the file + 1 (for null terminator)
    char buf[length + 1];
    buf[length] = '\0'; // Set the null terminator

    // Perform the read
    fread(buf, sizeof(char), length, f);

    /*
        The set_content() function takes four arguments
            1) The pointer to the 'HTTPResponse' struct on which to act
            2) A string representing the MIME type of the data
            3) The length of the data
            4) The data itself as a Bytes (a.k.a uint8_t[]) (char[] is fine too)
        
        This function calls the write_header function twice to set the
        "Content-Type" and "Content-Length" headers as appropriate, however
        this function is exposed to you as a user of the library, so you can make
        these function calls yourself if you wish. 
    */
    set_content(res, "text/html", length + 1, (Bytes)buf);

    return; // Unnecessary, but here for clarity
}

/*
    The STATIC() macro provides a shorthand for the functionality we provided above.
    The three arguments are as follows:
        1) The function name
        2) The filepath of the file to serve
        3) The MIME type of the file contents

    Handlers defined with STATIC() can be assigned to routes using the route() function like any other
*/

STATIC(get_static, "example1.html", "text/html");

/*
    We're also going to host an image called image.jpg which we refer to in our HTML page.
    We can define this handler using STATIC() in the same way.
    Note that jpeg images use the "image/jpg" MIME type.
*/

STATIC(get_image, "image.jpg", "image/jpg");

int main(int argc, char **argv) {

    // new_httpserver() returns a pointer to a HTTPServer struct or NULL if the process fails.
    // It takes a set of flags as an argument, and these can be OR'ed together to combine their 
    // effects
    HTTPServer *server = new_httpserver(KRONOS_HTTP_VERBOSE | KRONOS_HTTP_PRINT_HEAD);

    // If the server failed to create:
    if (server == NULL) {
        printf("Failed to create server.");
        exit(EXIT_FAILURE);
    }

    /*
        The route() function specifies which handler should be called for each request.
        It takes four arguments:
            1) A pointer to the 'HTTPServer' struct on which to act
            2) The method of the request
            3) The web route of the request
            4) A pointer to the handler function to be called
        
        If no handler is found for a specific request, the server returns a 404 Not Found error.
    */
    route(server, GET, "/", &get_root); // "localhost/" will be the web route that calls our get_root() function
    route(server, GET, "/static", &get_static); // "localhost/static" will display the same page, only using the handler we defined using STATIC()

    /*
        The RESOURCE() macro takes 3 arguments, which we'll call server, path and handler
        It then makes a call to route() like so:
            route(server, GET, path, &handler)
    */
    RESOURCE(server, "/image.jpg", &get_image);

    /*
        The run_server() function runs the server with two arguments:
            1) A pointer to the 'HTTPServer' struct to run
            2) The port number to run the server on
    */

    run_server(server, "0.0.0.0", 80);

}