// Example usage of cli_options.h in main.cpp
// This shows how to integrate the CLI parser into the existing main.cpp

#include "cli_options.h"
#include <iostream>
#include "Connection.h"
#include "objects.h"

using namespace std;
using namespace wpp;

int main(int argc, char* argv[]) {
    // Parse command-line options
    CliOptions options;
    if (!parseCliOptions(argc, argv, options)) {
        // Help/version was shown or error occurred
        return (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 ||
                             strcmp(argv[1], "-V") == 0 || strcmp(argv[1], "--version") == 0)) ? 0 : 1;
    }

    // Apply quiet mode
    if (options.quiet) {
        // Redirect cout to null or use a custom logger
    }

    if (!options.quiet) {
        cout << endl << "---- Creating required components ----" << endl;
    }

    // Use CLI options instead of hardcoded values
    Connection connection(options.local_port, options.address_family);

    // Client initialization with CLI endpoint name
    if (!options.quiet) {
        cout << endl << "---- Creating WppClient ----" << endl;
        cout << "WppClient name: " << options.endpoint_name << endl;
        cout << "Server URI: " << options.server_uri << endl;
        cout << "Security Mode: ";
        switch (options.security_mode) {
            case SecurityMode::NONE: cout << "NO_SEC" << endl; break;
            case SecurityMode::PSK:  cout << "PSK" << endl; break;
            case SecurityMode::RPK:  cout << "RPK" << endl; break;
            case SecurityMode::CERT: cout << "CERT" << endl; break;
        }
    }

    WppClient::create({options.endpoint_name, "", ""}, connection, wppErrorHandler);
    WppClient *client = WppClient::takeOwnershipBlocking();

    // Initialize objects
    // ... rest of initialization code from original main.cpp ...

    // The security initialization could be modified to use CLI options:
    // - options.security_mode
    // - options.psk_identity
    // - options.psk_key
    // - options.rpk_public_key / options.rpk_private_key
    // - options.cert_file / options.key_file / options.ca_file

    return 0;
}
