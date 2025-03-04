 /*
  * Copyright (c) 2021, Net-snmp authors
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  *
  * * Redistributions of source code must retain the above copyright notice, this
  *   list of conditions and the following disclaimer.
  *
  * * Redistributions in binary form must reproduce the above copyright notice,
  *   this list of conditions and the following disclaimer in the documentation
  *   and/or other materials provided with the distribution.
  *
  * * Neither the name of the copyright holder nor the names of its
  *   contributors may be used to endorse or promote products derived from
  *   this software without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

int SecmodInMsg_CB (struct snmp_secmod_incoming_params *sp1) {
    return SNMPERR_SUCCESS;
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
    if (getenv("NETSNMP_DEBUGGING") != NULL) {
        /*
         * Turn on all debugging, to help understand what
         * bits of the parser are running.
         */
        snmp_enable_stderrlog();
        snmp_set_do_debugging(1);
        debug_register_tokens("");
    }

    struct snmp_secmod_def *sndef = SNMP_MALLOC_STRUCT(snmp_secmod_def);
    sndef->decode = SecmodInMsg_CB;
    register_sec_mod(-1, "modname", sndef);
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    size_t bytes_remaining = size;
    netsnmp_pdu *pdu = SNMP_MALLOC_TYPEDEF(netsnmp_pdu);

    netsnmp_session sess = { };
    snmpv3_parse(pdu, (unsigned char *)data, &bytes_remaining, NULL, &sess);
    snmp_free_pdu(pdu);

    pdu = SNMP_MALLOC_TYPEDEF(netsnmp_pdu);
    memset(&sess, 0, sizeof(sess));
    snmp_parse(NULL, &sess, pdu, (unsigned char *)data, size);
    snmp_free_pdu(pdu);

    return 0;
}
