/*
 * Copyright (C) 2019 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

#define BTSTACK_FILE__ "mesh_configuration_client.c"

#include <string.h>
#include <stdio.h>

#include "mesh/mesh_configuration_client.h"

#include "bluetooth_company_id.h"
#include "btstack_debug.h"
#include "btstack_memory.h"
#include "btstack_util.h"

#include "mesh/mesh_access.h"
#include "mesh/mesh_foundation.h"
#include "mesh/mesh_generic_model.h"
#include "mesh/mesh_keys.h"
#include "mesh/mesh_network.h"
#include "mesh/mesh_upper_transport.h"


static void mesh_configuration_client_send_acknowledged(uint16_t src, uint16_t dest, uint16_t netkey_index, uint16_t appkey_index, mesh_pdu_t *pdu, uint32_t ack_opcode){
    uint8_t  ttl  = mesh_foundation_default_ttl_get();
    mesh_upper_transport_setup_access_pdu_header(pdu, netkey_index, appkey_index, ttl, src, dest, 0);
    mesh_access_send_acknowledged_pdu(pdu, mesh_access_acknowledged_message_retransmissions(), ack_opcode);
}

static uint8_t mesh_access_validate_envelop_params(mesh_model_t * mesh_model, uint16_t dest, uint16_t netkey_index, uint16_t appkey_index){
    btstack_assert(mesh_model != NULL);
    // TODO: validate other params
    UNUSED(dest);
    UNUSED(netkey_index);
    UNUSED(appkey_index);

    return ERROR_CODE_SUCCESS;
}

// Configuration client messages

static const mesh_access_message_t mesh_configuration_client_beacon_get = {
        MESH_FOUNDATION_OPERATION_BEACON_GET, ""
};
static const mesh_access_message_t mesh_configuration_client_beacon_set = {
        MESH_FOUNDATION_OPERATION_BEACON_SET, "1"
};


static const mesh_access_message_t mesh_configuration_client_composition_data_get = {
        MESH_FOUNDATION_OPERATION_COMPOSITION_DATA_GET, "1"
};


static const mesh_access_message_t mesh_configuration_client_default_ttl_get = {
        MESH_FOUNDATION_OPERATION_DEFAULT_TTL_GET, ""
};
static const mesh_access_message_t mesh_configuration_client_default_ttl_set = {
        MESH_FOUNDATION_OPERATION_DEFAULT_TTL_SET, "1"
};

#if 0
static const mesh_access_message_t mesh_configuration_client_gatt_proxy_get = {
        MESH_FOUNDATION_OPERATION_GATT_PROXY_GET, ""
};
static const mesh_access_message_t mesh_configuration_client_gatt_proxy_set = {
        MESH_FOUNDATION_OPERATION_GATT_PROXY_SET, "1"
};


static const mesh_access_message_t mesh_configuration_client_relay_get = {
        MESH_FOUNDATION_OPERATION_RELAY_GET, ""
};
static const mesh_access_message_t mesh_configuration_client_relay_set = {
        MESH_FOUNDATION_OPERATION_RELAY_SET, "11"
};


static const mesh_access_message_t mesh_configuration_client_publication_get = {
        MESH_FOUNDATION_OPERATION_MODEL_PUBLICATION_GET, "2m"
};
static const mesh_access_message_t mesh_configuration_client_publication_set = {
        MESH_FOUNDATION_OPERATION_MODEL_PUBLICATION_SET, "222111m"
};
static const mesh_access_message_t mesh_configuration_client_publication_virtual_address_set = {
        MESH_FOUNDATION_OPERATION_MODEL_PUBLICATION_VIRTUAL_ADDRESS_SET, "2P2111m"
};
#endif

uint8_t mesh_configuration_client_send_config_beacon_get(mesh_model_t * mesh_model, uint16_t dest, uint16_t netkey_index, uint16_t appkey_index){
    uint8_t status = mesh_access_validate_envelop_params(mesh_model, dest, netkey_index, appkey_index);
    if (status != ERROR_CODE_SUCCESS) return status;

    mesh_network_pdu_t * network_pdu = mesh_access_setup_unsegmented_message(&mesh_configuration_client_beacon_get);
    if (!network_pdu) return BTSTACK_MEMORY_ALLOC_FAILED;

    mesh_configuration_client_send_acknowledged(mesh_access_get_element_address(mesh_model), dest, netkey_index, appkey_index, (mesh_pdu_t *) network_pdu, MESH_FOUNDATION_OPERATION_BEACON_STATUS);
    return ERROR_CODE_SUCCESS;
}

uint8_t mesh_configuration_client_send_config_beacon_set(mesh_model_t * mesh_model, uint16_t dest, uint16_t netkey_index, uint16_t appkey_index, uint8_t beacon){
    uint8_t status = mesh_access_validate_envelop_params(mesh_model, dest, netkey_index, appkey_index);
    if (status != ERROR_CODE_SUCCESS) return status;

    if (beacon > 1) return ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE;

    mesh_network_pdu_t * network_pdu = mesh_access_setup_unsegmented_message(&mesh_configuration_client_beacon_set, beacon);
    if (!network_pdu) return BTSTACK_MEMORY_ALLOC_FAILED;

    mesh_configuration_client_send_acknowledged(mesh_access_get_element_address(mesh_model), dest, netkey_index, appkey_index, (mesh_pdu_t *) network_pdu, MESH_FOUNDATION_OPERATION_BEACON_STATUS);
    return ERROR_CODE_SUCCESS;
}

uint8_t mesh_configuration_client_send_composition_data_get(mesh_model_t * mesh_model, uint16_t dest, uint16_t netkey_index, uint16_t appkey_index, uint8_t page){
    uint8_t status = mesh_access_validate_envelop_params(mesh_model, dest, netkey_index, appkey_index);
    if (status != ERROR_CODE_SUCCESS) return status;

    mesh_network_pdu_t * network_pdu = mesh_access_setup_unsegmented_message(&mesh_configuration_client_composition_data_get, page);
    if (!network_pdu) return BTSTACK_MEMORY_ALLOC_FAILED;

    mesh_configuration_client_send_acknowledged(mesh_access_get_element_address(mesh_model), dest, netkey_index, appkey_index, (mesh_pdu_t *) network_pdu, MESH_FOUNDATION_OPERATION_COMPOSITION_DATA_GET);
    return ERROR_CODE_SUCCESS;
}

uint8_t mesh_configuration_client_send_default_ttl_get(mesh_model_t * mesh_model, uint16_t dest, uint16_t netkey_index, uint16_t appkey_index){
    uint8_t status = mesh_access_validate_envelop_params(mesh_model, dest, netkey_index, appkey_index);
    if (status != ERROR_CODE_SUCCESS) return status;

    mesh_network_pdu_t * network_pdu = mesh_access_setup_unsegmented_message(&mesh_configuration_client_default_ttl_get);
    if (!network_pdu) return BTSTACK_MEMORY_ALLOC_FAILED;

    mesh_configuration_client_send_acknowledged(mesh_access_get_element_address(mesh_model), dest, netkey_index, appkey_index, (mesh_pdu_t *) network_pdu, MESH_FOUNDATION_OPERATION_DEFAULT_TTL_GET);
    return ERROR_CODE_SUCCESS;
}

uint8_t mesh_configuration_client_send_default_ttl_set(mesh_model_t * mesh_model, uint16_t dest, uint16_t netkey_index, uint16_t appkey_index, uint8_t ttl){
    uint8_t status = mesh_access_validate_envelop_params(mesh_model, dest, netkey_index, appkey_index);
    if (status != ERROR_CODE_SUCCESS) return status;

    if (ttl == 0x01 || ttl >= 0x80) return ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE;

    mesh_network_pdu_t * network_pdu = mesh_access_setup_unsegmented_message(&mesh_configuration_client_default_ttl_set, ttl);
    if (!network_pdu) return BTSTACK_MEMORY_ALLOC_FAILED;

    mesh_configuration_client_send_acknowledged(mesh_access_get_element_address(mesh_model), dest, netkey_index, appkey_index, (mesh_pdu_t *) network_pdu, MESH_FOUNDATION_OPERATION_DEFAULT_TTL_SET);
    return ERROR_CODE_SUCCESS;
}

// Model Operations
static void mesh_configuration_client_beacon_status_handler(mesh_model_t *mesh_model, mesh_pdu_t * pdu){
    mesh_access_parser_state_t parser;
    mesh_access_parser_init(&parser, (mesh_pdu_t*) pdu);
    
    uint8_t beacon_status = mesh_access_parser_get_u8(&parser);

    uint8_t event[7] = {HCI_EVENT_MESH_META, 5, MESH_SUBEVENT_FOUNDATION_BEACON_STATUS};
    int pos = 3;
    // dest
    little_endian_store_16(event, pos, mesh_pdu_src(pdu));
    pos += 2;
    event[pos++] = ERROR_CODE_SUCCESS;
    event[pos++] = beacon_status;
    
    (*mesh_model->model_packet_handler)(HCI_EVENT_PACKET, 0, event, pos);
    mesh_access_message_processed(pdu);
}

static void mesh_configuration_client_composition_data_status_handler(mesh_model_t *mesh_model, mesh_pdu_t * pdu){
    mesh_access_parser_state_t parser;
    mesh_access_parser_init(&parser, (mesh_pdu_t*) pdu);
    
    uint8_t page = mesh_access_parser_get_u8(&parser);
    uint16_t cid = mesh_access_parser_get_u16(&parser);
    uint16_t pid = mesh_access_parser_get_u16(&parser);
    uint16_t vid = mesh_access_parser_get_u16(&parser);
    uint16_t crpl = mesh_access_parser_get_u16(&parser);
    uint16_t features = mesh_access_parser_get_u16(&parser);

    // TODO: list of element descriptions, see Table 4.4

    uint8_t event[17] = {HCI_EVENT_MESH_META, 5, MESH_SUBEVENT_FOUNDATION_COMPOSITION_DATA_STATUS};
    int pos = 3;
    // dest
    little_endian_store_16(event, pos, mesh_pdu_src(pdu));
    pos += 2;
    event[pos++] = ERROR_CODE_SUCCESS;
    event[pos++] = page;
    
    little_endian_store_16(event, pos, cid);
    pos += 2;
    little_endian_store_16(event, pos, pid);
    pos += 2;
    little_endian_store_16(event, pos, vid);
    pos += 2;
    little_endian_store_16(event, pos, crpl);
    pos += 2;
    little_endian_store_16(event, pos, features);
    pos += 2;

    (*mesh_model->model_packet_handler)(HCI_EVENT_PACKET, 0, event, pos);
    mesh_access_message_processed(pdu);
}

static void mesh_configuration_client_default_ttl_handler(mesh_model_t *mesh_model, mesh_pdu_t * pdu){
    mesh_access_parser_state_t parser;
    mesh_access_parser_init(&parser, (mesh_pdu_t*) pdu);
    
    uint8_t default_ttl = mesh_access_parser_get_u8(&parser);

    uint8_t event[7] = {HCI_EVENT_MESH_META, 5, MESH_SUBEVENT_FOUNDATION_DEFAULT_TTL_STATUS};
    int pos = 3;
    // dest
    little_endian_store_16(event, pos, mesh_pdu_src(pdu));
    pos += 2;
    event[pos++] = ERROR_CODE_SUCCESS;
    event[pos++] = default_ttl;
    
    (*mesh_model->model_packet_handler)(HCI_EVENT_PACKET, 0, event, pos);
    mesh_access_message_processed(pdu);
}

const static mesh_operation_t mesh_configuration_client_model_operations[] = {
    { MESH_FOUNDATION_OPERATION_BEACON_STATUS,           1, mesh_configuration_client_beacon_status_handler },
    { MESH_FOUNDATION_OPERATION_COMPOSITION_DATA_STATUS, 1, mesh_configuration_client_composition_data_status_handler },
    { MESH_FOUNDATION_OPERATION_DEFAULT_TTL_STATUS,      1, mesh_configuration_client_default_ttl_handler },
    { 0, 0, NULL }
};

const mesh_operation_t * mesh_configuration_client_get_operations(void){
    return mesh_configuration_client_model_operations;
}

void mesh_configuration_client_register_packet_handler(mesh_model_t *configuration_client_model, btstack_packet_handler_t events_packet_handler){
    btstack_assert(events_packet_handler != NULL);
    btstack_assert(configuration_client_model != NULL);
    
    configuration_client_model->model_packet_handler = events_packet_handler;
}
