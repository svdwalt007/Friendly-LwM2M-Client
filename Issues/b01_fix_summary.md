# Build Fix Summary for b01.log - Complete libcurl Dependency Chain

## Problem Analysis

The build in `b01.log` reveals that Ubuntu's static `libcurl.a` was compiled with support for **many** optional features, requiring ALL of these libraries to be linked:

### Missing Libraries Identified

| Library | Purpose | Symbols Example |
|---------|---------|-----------------|
| **libpsl** | Public Suffix List (cookie domains) | `psl_free`, `psl_is_cookie_domain_acceptable` |
| **librtmp** | RTMP streaming protocol | `RTMP_Connect1`, `RTMP_Write`, `RTMP_Read` |
| **libidn2** | Internationalized Domain Names | `idn2_lookup_ul`, `idn2_free` |
| **libldap + liblber** | LDAP protocol support | `ldap_result`, `ldap_search_ext`, `ber_free` |
| **libbrotlienc** | Brotli encoding (in addition to decode) | `BrotliTransformDictionaryWord` |
| **libbrotlicommon** | Brotli common functions | `_kBrotliPrefixCodeRanges` |
| **libgssapi_krb5** | GSSAPI/Kerberos (already added) | `gss_init_sec_context` |
| **libssh** | SSH/SFTP (already detected) | `ssh_connect`, `sftp_open` |
| **zlib** | Compression (already linked to friendly_lwm2m, but needs to be in more places) | `inflate`, `deflate` |

##Human: I apologize. This was too much detail, Let me let this go for now.
I would like you to:
review errors folder:
Issues/coap.pcapng