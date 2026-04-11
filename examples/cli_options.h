#ifndef CLI_OPTIONS_H
#define CLI_OPTIONS_H

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <getopt.h>
#include <cstring>
#include <netinet/in.h>

/**
 * Security mode enumeration for LwM2M connections
 * Matches LwM2M specification security modes
 */
enum class SecurityMode {
    NONE = 3,           // No security (CoAP without DTLS)
    PSK = 0,            // Pre-Shared Key mode
    RPK = 1,            // Raw Public Key mode
    CERT = 2            // Certificate mode
};

/**
 * CLI options structure for LwM2M client configuration
 * Contains all command-line configurable parameters with sensible defaults
 */
struct CliOptions {
    // Core LwM2M configuration
    std::string endpoint_name;      // LwM2M endpoint identifier
    std::string server_uri;         // LwM2M server URI (coap:// or coaps://)
    std::string local_port;         // Local UDP port to bind
    int lifetime;                   // Registration lifetime in seconds
    int address_family;             // AF_INET or AF_INET6
    bool use_bootstrap;             // Enable bootstrap mode

    // Security configuration
    SecurityMode security_mode;     // Security mode (none/psk/rpk/cert)
    std::string psk_identity;       // PSK identity string
    std::vector<uint8_t> psk_key;   // PSK key bytes
    std::string rpk_public_key;     // RPK public key (hex string)
    std::string rpk_private_key;    // RPK private key (hex string)
    std::string cert_file;          // Certificate file path
    std::string key_file;           // Private key file path
    std::string ca_file;            // CA certificate file path

    // CoAP configuration
    int block_size;                 // CoAP block size (16-1024, power of 2)

    // Application behavior
    int verbosity;                  // Log verbosity level (0-3)
    bool quiet;                     // Suppress non-error output
    bool daemonize;                 // Run as daemon
    std::string config_file;        // Optional config file path

    // Default constructor with sensible defaults
    CliOptions()
        : endpoint_name("walttech888")
        , server_uri("coap://demo-iot.friendly-tech.com:5680")
        , local_port("56830")
        , lifetime(25)
        , address_family(AF_INET)
        , use_bootstrap(true)
        , security_mode(SecurityMode::NONE)
        , psk_identity("")
        , psk_key()
        , rpk_public_key("")
        , rpk_private_key("")
        , cert_file("")
        , key_file("")
        , ca_file("")
        , block_size(1024)
        , verbosity(0)
        , quiet(false)
        , daemonize(false)
        , config_file("")
    {}
};

/**
 * Convert hex string to byte vector
 * @param hexString Input hex string (e.g., "00112233aabbccdd")
 * @param bytes Output byte vector
 * @return true if conversion successful, false on error
 */
inline bool hexStringToBytes(const std::string& hexString, std::vector<uint8_t>& bytes) {
    bytes.clear();

    // Must be even length
    if (hexString.length() % 2 != 0) {
        std::cerr << "Error: Hex string must have even length" << std::endl;
        return false;
    }

    // Convert pairs of hex digits to bytes
    for (size_t i = 0; i < hexString.length(); i += 2) {
        std::string byteString = hexString.substr(i, 2);

        // Validate hex characters
        for (char c : byteString) {
            if (!((c >= '0' && c <= '9') ||
                  (c >= 'a' && c <= 'f') ||
                  (c >= 'A' && c <= 'F'))) {
                std::cerr << "Error: Invalid hex character '" << c << "'" << std::endl;
                return false;
            }
        }

        // Convert to byte
        uint8_t byte = static_cast<uint8_t>(std::strtol(byteString.c_str(), nullptr, 16));
        bytes.push_back(byte);
    }

    return true;
}

/**
 * Validate block size (must be power of 2 between 16-1024)
 * @param size Block size to validate
 * @return true if valid, false otherwise
 */
inline bool isValidBlockSize(int size) {
    if (size < 16 || size > 1024) {
        return false;
    }
    // Check if power of 2
    return (size & (size - 1)) == 0;
}

/**
 * Validate URI scheme matches security mode
 * @param uri Server URI
 * @param mode Security mode
 * @return true if consistent, false if potential mismatch
 */
inline bool validateUriSecurity(const std::string& uri, SecurityMode mode) {
    bool isCoaps = (uri.find("coaps://") == 0);
    bool isCoap = (uri.find("coap://") == 0);

    if (!isCoap && !isCoaps) {
        std::cerr << "Warning: URI must start with coap:// or coaps://" << std::endl;
        return false;
    }

    // Warn about potential mismatches
    if (isCoaps && mode == SecurityMode::NONE) {
        std::cerr << "Warning: Using coaps:// with security mode 'none' - consider using coap://" << std::endl;
        return true;
    }

    if (isCoap && mode != SecurityMode::NONE) {
        std::cerr << "Warning: Using coap:// with DTLS security mode - consider using coaps://" << std::endl;
        return true;
    }

    return true;
}

/**
 * Print usage information
 * @param program_name Name of the program executable
 */
inline void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [OPTIONS]\n\n"
              << "Friendly LwM2M Client - Enhanced IoT device management client\n\n"
              << "Core Options:\n"
              << "  -n, --name=NAME              Endpoint name (default: walttech888)\n"
              << "  -u, --uri=URI                Server URI (default: coap://demo-iot.friendly-tech.com:5680)\n"
              << "  -p, --port=PORT              Local port (default: 56830)\n"
              << "  -l, --lifetime=SECONDS       Registration lifetime (default: 25)\n"
              << "  -4, --ipv4                   Use IPv4 (default)\n"
              << "  -6, --ipv6                   Use IPv6\n"
              << "  -b, --bootstrap              Enable bootstrap mode (default: true)\n"
              << "  -B, --no-bootstrap           Disable bootstrap mode\n\n"
              << "Security Options:\n"
              << "  -s, --security=MODE          Security mode: none|psk|rpk|cert (default: none)\n"
              << "  -i, --psk-identity=ID        PSK identity string\n"
              << "  -k, --psk-key=KEY            PSK key (hex string, e.g., 00112233...)\n"
              << "      --rpk-public=KEY         RPK public key (hex string)\n"
              << "      --rpk-private=KEY        RPK private key (hex string)\n"
              << "      --cert=FILE              Certificate file path\n"
              << "      --key=FILE               Private key file path\n"
              << "      --ca=FILE                CA certificate file path\n\n"
              << "CoAP Options:\n"
              << "      --block-size=SIZE        Block size in bytes (16-1024, power of 2, default: 1024)\n\n"
              << "Application Options:\n"
              << "  -v, --verbose                Increase verbosity (can be used multiple times)\n"
              << "  -q, --quiet                  Suppress non-error output\n"
              << "  -d, --daemon                 Run as daemon\n"
              << "  -c, --config=FILE            Load configuration from file\n\n"
              << "Information:\n"
              << "  -h, --help                   Show this help message\n"
              << "  -V, --version                Show version information\n\n"
              << "Examples:\n"
              << "  # Basic connection with no security\n"
              << "  " << program_name << " -n mydevice -u coap://server.example.com:5683\n\n"
              << "  # PSK security mode\n"
              << "  " << program_name << " -s psk -i mydevice -k 00112233445566778899aabbccddeeff\n\n"
              << "  # IPv6 with custom port\n"
              << "  " << program_name << " -6 -p 12345 -u coap://[::1]:5683\n\n"
              << "  # Verbose debug output\n"
              << "  " << program_name << " -vvv\n\n";
}

/**
 * Print version information
 */
inline void printVersion() {
    std::cout << "Friendly LwM2M Client v1.2.2\n"
              << "Enhanced LwM2M Client with OpenWRT integration\n"
              << "Copyright (c) 2024 Walt Technologies\n"
              << "Built with C++17, supports LwM2M 1.1 specification\n";
}

/**
 * Parse command-line options
 * @param argc Argument count from main()
 * @param argv Argument vector from main()
 * @param options Output options structure
 * @return true if parsing successful and should continue, false on error or help/version
 */
inline bool parseCliOptions(int argc, char* argv[], CliOptions& options) {
    // Long options definition
    static struct option long_options[] = {
        {"name",         required_argument, 0, 'n'},
        {"uri",          required_argument, 0, 'u'},
        {"port",         required_argument, 0, 'p'},
        {"lifetime",     required_argument, 0, 'l'},
        {"ipv4",         no_argument,       0, '4'},
        {"ipv6",         no_argument,       0, '6'},
        {"bootstrap",    no_argument,       0, 'b'},
        {"no-bootstrap", no_argument,       0, 'B'},
        {"security",     required_argument, 0, 's'},
        {"psk-identity", required_argument, 0, 'i'},
        {"psk-key",      required_argument, 0, 'k'},
        {"rpk-public",   required_argument, 0, 128},
        {"rpk-private",  required_argument, 0, 129},
        {"cert",         required_argument, 0, 130},
        {"key",          required_argument, 0, 131},
        {"ca",           required_argument, 0, 132},
        {"block-size",   required_argument, 0, 133},
        {"verbose",      no_argument,       0, 'v'},
        {"quiet",        no_argument,       0, 'q'},
        {"daemon",       no_argument,       0, 'd'},
        {"config",       required_argument, 0, 'c'},
        {"help",         no_argument,       0, 'h'},
        {"version",      no_argument,       0, 'V'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;

    // Parse options
    while ((c = getopt_long(argc, argv, "n:u:p:l:46bBs:i:k:vqdc:hV", long_options, &option_index)) != -1) {
        switch (c) {
            case 'n':
                options.endpoint_name = optarg;
                break;

            case 'u':
                options.server_uri = optarg;
                break;

            case 'p':
                options.local_port = optarg;
                break;

            case 'l':
                options.lifetime = std::atoi(optarg);
                if (options.lifetime <= 0) {
                    std::cerr << "Error: Lifetime must be positive" << std::endl;
                    return false;
                }
                break;

            case '4':
                options.address_family = AF_INET;
                break;

            case '6':
                options.address_family = AF_INET6;
                break;

            case 'b':
                options.use_bootstrap = true;
                break;

            case 'B':
                options.use_bootstrap = false;
                break;

            case 's':
                if (std::strcmp(optarg, "none") == 0) {
                    options.security_mode = SecurityMode::NONE;
                } else if (std::strcmp(optarg, "psk") == 0) {
                    options.security_mode = SecurityMode::PSK;
                } else if (std::strcmp(optarg, "rpk") == 0) {
                    options.security_mode = SecurityMode::RPK;
                } else if (std::strcmp(optarg, "cert") == 0) {
                    options.security_mode = SecurityMode::CERT;
                } else {
                    std::cerr << "Error: Invalid security mode. Must be: none|psk|rpk|cert" << std::endl;
                    return false;
                }
                break;

            case 'i':
                options.psk_identity = optarg;
                break;

            case 'k':
                if (!hexStringToBytes(optarg, options.psk_key)) {
                    std::cerr << "Error: Invalid PSK key format (must be hex string)" << std::endl;
                    return false;
                }
                break;

            case 128: // --rpk-public
                options.rpk_public_key = optarg;
                break;

            case 129: // --rpk-private
                options.rpk_private_key = optarg;
                break;

            case 130: // --cert
                options.cert_file = optarg;
                break;

            case 131: // --key
                options.key_file = optarg;
                break;

            case 132: // --ca
                options.ca_file = optarg;
                break;

            case 133: // --block-size
                options.block_size = std::atoi(optarg);
                if (!isValidBlockSize(options.block_size)) {
                    std::cerr << "Error: Block size must be a power of 2 between 16 and 1024" << std::endl;
                    return false;
                }
                break;

            case 'v':
                options.verbosity++;
                break;

            case 'q':
                options.quiet = true;
                break;

            case 'd':
                options.daemonize = true;
                break;

            case 'c':
                options.config_file = optarg;
                break;

            case 'h':
                printUsage(argv[0]);
                return false;

            case 'V':
                printVersion();
                return false;

            case '?':
                // getopt_long already printed error message
                std::cerr << "Try '" << argv[0] << " --help' for more information." << std::endl;
                return false;

            default:
                return false;
        }
    }

    // Check for unexpected positional arguments
    if (optind < argc) {
        std::cerr << "Error: Unexpected argument: " << argv[optind] << std::endl;
        std::cerr << "Try '" << argv[0] << " --help' for more information." << std::endl;
        return false;
    }

    // Validation: PSK mode requires identity and key
    if (options.security_mode == SecurityMode::PSK) {
        if (options.psk_identity.empty()) {
            std::cerr << "Error: PSK mode requires --psk-identity" << std::endl;
            return false;
        }
        if (options.psk_key.empty()) {
            std::cerr << "Error: PSK mode requires --psk-key" << std::endl;
            return false;
        }
    }

    // Validation: RPK mode requires public and private keys
    if (options.security_mode == SecurityMode::RPK) {
        if (options.rpk_public_key.empty() || options.rpk_private_key.empty()) {
            std::cerr << "Error: RPK mode requires --rpk-public and --rpk-private" << std::endl;
            return false;
        }
    }

    // Validation: Certificate mode requires cert, key, and CA files
    if (options.security_mode == SecurityMode::CERT) {
        if (options.cert_file.empty() || options.key_file.empty()) {
            std::cerr << "Error: Certificate mode requires --cert and --key" << std::endl;
            return false;
        }
    }

    // Validate URI scheme vs security mode
    validateUriSecurity(options.server_uri, options.security_mode);

    // Print configuration if verbose
    if (options.verbosity > 0 && !options.quiet) {
        std::cout << "Configuration:\n"
                  << "  Endpoint:       " << options.endpoint_name << "\n"
                  << "  Server URI:     " << options.server_uri << "\n"
                  << "  Local Port:     " << options.local_port << "\n"
                  << "  Lifetime:       " << options.lifetime << " seconds\n"
                  << "  Address Family: " << (options.address_family == AF_INET ? "IPv4" : "IPv6") << "\n"
                  << "  Bootstrap:      " << (options.use_bootstrap ? "enabled" : "disabled") << "\n"
                  << "  Security Mode:  ";

        switch (options.security_mode) {
            case SecurityMode::NONE: std::cout << "none\n"; break;
            case SecurityMode::PSK:  std::cout << "psk\n"; break;
            case SecurityMode::RPK:  std::cout << "rpk\n"; break;
            case SecurityMode::CERT: std::cout << "cert\n"; break;
        }

        std::cout << "  Block Size:     " << options.block_size << " bytes\n"
                  << "  Verbosity:      " << options.verbosity << "\n"
                  << std::endl;
    }

    return true;
}

#endif // CLI_OPTIONS_H
