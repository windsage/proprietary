/*
 * Copyright (c) 2016, 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "common.h"
#include "ven_cmd_tool.h"
#include "nl_cmd_wrapper.h"

#ifdef CONFIG_SUPPORT_LIBROXML
#include <roxml.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#else
#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#endif

#ifdef __IPQ__
const char* const short_options = "h:i:f:l:";
#else
const char* const short_options = "h:i:f:";
#endif

#ifdef CONFIG_SUPPORT_LIBROXML
#define MAX_NAME_LEN 64
#else
/* These are node type definitions from xmlreader.h library */
#define NODE_TYPE_ELEMENT 1
#define NODE_TYPE_COMMENT 8
#define NODE_TYPE_END_ELEMENT 15
#endif

#if defined(__IPQ__) || defined(SDX_MCC)
/* This command is at index 63(?), getlongoption returns 63
 * if command is not available.
 * whatever command is there at index 63 can not work.
 *
 */
#define RESERVE_ASCII_VALUE 63
#define ARGV_COMMAND_OFFSET 10
char reservecmd[FILE_NAME_LEN];
int display_all_commands = 0; /* switch to display all avaialble commands */
#endif /* __IPQ__ || SDX_MCC */

#ifdef CONFIG_SUPPORT_LIBROXML
size_t getFilesize(const char* filename)
{
    struct stat st = {0};
    if (filename) {
        stat(filename, &st);
        return st.st_size;
    } else return 0;
}
#endif

static enum attr_type getAttrTypeenum(char *value)
{
    enum attr_type type = INVALID_ATTR;
    if(!strcasecmp(value, "flag")) {
        type = FLAG;
    } else if(!strcasecmp(value, "u8")) {
        type = U8;
    } else if(!strcasecmp(value, "u16")) {
        type = U16;
    } else if(!strcasecmp(value, "u32")) {
        type = U32;
    } else if(!strcasecmp(value, "u64")) {
        type = U64;
    } else if(!strcasecmp(value, "s8")) {
        type = S8;
    } else if(!strcasecmp(value, "s16")) {
        type = S16;
    } else if(!strcasecmp(value, "s32")) {
        type = S32;
    } else if(!strcasecmp(value, "s64")) {
        type = S64;
    } else if(!strcasecmp(value, "string")) {
        type = STRING;
    } else if(!strcasecmp(value, "mac_addr")) {
        type = MAC_ADDR;
    } else if(!strcasecmp(value, "blob")) {
        type = BLOB;
    } else if(!strcasecmp(value, "nested")) {
        type = NESTED;
    } else {
        printf("unknown attr type : %s\n", value);
    }

    return type;
}


static enum cmd_type getCmdType(char *value)
{
    enum cmd_type type = INVALID_CMD;

    if(!strcasecmp(value, "VendorCmd")) {
        type = VEN_CMD;
    } else if(!strcasecmp(value, "VendorRsp")) {
        type = RESPONSE;
    } else if(!strcasecmp(value, "VendorEvent")) {
        type = VEN_EVENT;
    } else if(!strcasecmp(value, "NLEvent")) {
        type = NL_EVENT;
    } else if(!strcasecmp(value, "switch")) {
        type = SWITCH;
    } else if(!strcasecmp(value, "case")) {
        type = CASE;
    } else if(!strcasecmp(value, "attribute")) {
        type = ATTRIBUTE;
    } else {
        printf("unknown cmd type : %s\n", value);
    }

    return type;
}


static void parseDefaultAttrList(union default_values *default_val,
    const char *data)
{
    struct default_list_t *node=NULL, *list = NULL;
    char *values, *id, delim[2] = {',', 0};
    char *saveptr;

    values = (char *)malloc(strlen(data) + 1);
    if (!values) {
        printf("Failed to allocate memory for values\n");
        return;
    }
    memcpy(values, data, strlen(data)+1);

#if !defined(__IPQ__) && !defined(SDX_MCC)
    printf("Default attributes: %s\n", values);
#endif /* __IPQ__ || SDX_MCC */
    id = strtok_r(values, delim, &saveptr);
    while (id) {
        node = (struct default_list_t *)malloc(sizeof(struct default_list_t));
        if (!node) {
            printf("Failed to allocate memory for node\n");
            goto done;
        }
        node->default_id = atoi(id);
        node->next = list;
        list = node;
        id = strtok_r(NULL, delim, &saveptr);
    }

    default_val->default_list = list;

done:
    free(values);
}

static char *enumToString(enum option_type type)
{
    char *cmdtype;
    switch (type)
    {
        case O_HELP:
            cmdtype = "HELP";
            break;
        case O_COMMAND:
            cmdtype = "START_CMD";
            break;
        case O_END_CMD:
            cmdtype = "END_CMD";
            break;
        case O_RESPONSE:
            cmdtype = "RESPONSE";
            break;
        case O_END_RSP:
            cmdtype = "END_RSP";
            break;
        case O_EVENT:
            cmdtype = "EVENT";
            break;
        case O_END_EVENT:
            cmdtype = "END_EVENT";
            break;
        case O_NESTED:
            cmdtype = "NESTED";
            break;
        case O_SWITCH:
            cmdtype = "SWITCH";
            break;
        case O_CASE:
            cmdtype = "CASE";
            break;
        case O_NESTED_AUTO:
            cmdtype = "NESTED_AUTO";
            break;
        case O_END_ATTR:
            cmdtype = "END_ATTR";
            break;
        case O_END_NESTED_AUTO:
            cmdtype = "END_NESTED_AUTO";
            break;
        default:
            printf("invalid enum value : %d\n", type);
            cmdtype = NULL;
    }
    return cmdtype;
}


static void populateCLIOptions(struct cmd_params *cmd)
{
    enum option_type j;
    char *cmdname;

    for (j=O_HELP; j< O_CMD_LAST; j++, cmd->num_entries++) {
        cmd->entry[cmd->num_entries].ctrl_option = j;

        cmdname = enumToString(j);
        if (!cmdname) {
            printf("command name not found option_type : %d\n", j);
            return;
        }
        cmd->long_options[cmd->num_entries].name =
                                            (char *)malloc(strlen(cmdname)+1);
        memcpy((char *)cmd->long_options[cmd->num_entries].name,
               cmdname,
               strlen(cmdname)+1);
        cmd->long_options[cmd->num_entries].val = cmd->num_entries;
        /* No need to populate other elements for now */
#ifdef DEBUG
        printf("num_entries: %d, ctrl_option: %d-%s\n",
               cmd->num_entries, j, cmdname);
#endif
    }
}


#ifdef CONFIG_SUPPORT_LIBROXML
static int fill_content(node_t *node, struct cmd_params *cmd)
{
    char value[MAX_NAME_LEN] = {0};
    int idx = 0, size;
    node_t *attr;

    if (!cmd) {
        printf("cmd is NULL\n");
        return -1;
    }
    idx = cmd->num_entries;

    roxml_get_name(node, &value[0], MAX_NAME_LEN);
    cmd->entry[idx].xml_entry = getCmdType(&value[0]);

    attr = roxml_get_attr(node, "name", 0);
    if (attr) {
        roxml_get_content(attr, &value[0], MAX_NAME_LEN, &size);
        if (strlen(value) > 0) {
            cmd->long_options[idx].name = (char *)malloc(strlen(value) + 1);
            if (!cmd->long_options[idx].name) {
                printf("Failed to allocate memory for name\n");
            } else {
                memcpy((char*)cmd->long_options[idx].name, value,
                        strlen(value) + 1);
                cmd->long_options[idx].val = idx;
            }
        }
    }

    attr = roxml_get_attr(node, "ID", 0);
    if (attr) {
        roxml_get_content(attr, &value[0], MAX_NAME_LEN, &size);
        if (strlen(value) > 0) {
            cmd->attr_id[idx] = atoi((const char *)value);
        }
    }


    attr = roxml_get_attr(node, "TYPE", 0);
    if (attr) {
        roxml_get_content(attr, &value[0], MAX_NAME_LEN, &size);
        if (strlen(value) > 0) {
            cmd->data_type[idx] = getAttrTypeenum(value);
            if (cmd->data_type[idx] >= U8 && cmd->data_type[idx] <= BLOB) {
                cmd->long_options[idx].has_arg = 1;
            }
        }
    }

    attr = roxml_get_attr(node, "DEFAULTS", 0);
    if (attr) {
        roxml_get_content(attr, &value[0], MAX_NAME_LEN, &size);
        if (strlen(value) > 0) {
            parseDefaultAttrList(&cmd->default_val[idx], (const char *)value);
        }
    }

    attr = roxml_get_attr(node, "DEFAULT", 0);
    if (attr) {
        roxml_get_content(attr, &value[0], MAX_NAME_LEN, &size);
        if (strlen(value) > 0) {
            cmd->default_val[idx].val =
                (char *)malloc(strlen((const char *)value) + 1);

            if (!cmd->default_val[idx].val) {
                printf("Failed to allocate memory for default val\n");
            } else {
                memcpy((char *)cmd->default_val[idx].val, value,
                       strlen((const char *)value) + 1);
            }
        }
    }

    attr = roxml_get_attr(node, "ATTR_MAX", 0);
    if (attr) {
        roxml_get_content(attr, &value[0], MAX_NAME_LEN, &size);
        if (strlen(value) > 0) {
            cmd->attr_max[idx] = atoi((const char *)value);
        }
    }

    cmd->num_entries++;
    return 0;
}

static int fill_attr_content(node_t *node, struct cmd_params *cmds[])
{
    char name[MAX_NAME_LEN] = {0};
    int num_attrs, idx, ret = 0, num_chld, c_idx;
    node_t *child, *resp_chld;

    num_attrs = roxml_get_chld_nb(node);

    // fill node content first
    ret = fill_content(node, cmds[0]);
    for (idx = 0; idx < num_attrs; idx++) {
        child = roxml_get_chld(node, NULL, idx);

        roxml_get_name(child, name, MAX_NAME_LEN);

        if (strncmp(name, "Attribute", strlen("Attribute")) == 0){
            ret = fill_content(child, cmds[0]);
        } else if (strncmp(name, "VendorRsp", strlen("VendorRsp")) == 0) {
            ret = fill_content(child, cmds[1]);
            num_chld = roxml_get_chld_nb(child);

            // Loop over children for attributes as well
            for (c_idx = 0; c_idx < num_chld; c_idx++) {
                resp_chld = roxml_get_chld(child, NULL, c_idx);
                ret = fill_content(resp_chld, cmds[1]);
                if (ret)
                    return -1;
            }

        }
        if (ret)
            return -1;
    }

    return 0;
}

static int parseRoXMLCommands(node_t *root, struct cmd_params *cmds[],
                              char *user_command)
{
    char name[MAX_NAME_LEN] = {0}, buf[MAX_NAME_LEN] = {0};
    node_t *node = NULL, *attr = NULL;
    int idx = 0, ret = 0, cmd_num = 0, size = 0;
    cmd_num = roxml_get_chld_nb(root);

    for (idx = 0; idx < cmd_num; idx++) {

        // Get command node from index
        node = roxml_get_chld(root, NULL, idx);
        if (!node) {
            break;
        }

        // Check if it is VendorCmd
        roxml_get_name(node, &name[0], MAX_NAME_LEN);
        if (strncmp(name, "VendorCmd", 9) == 0) {
            if (user_command) {
                attr = roxml_get_attr(node, "name", 0);
                roxml_get_content(attr, &buf[0], MAX_NAME_LEN, &size);

                // Compare both strings only if length matches.
                if ((strlen(user_command) == strlen(buf)) &&
                    (strncmp(&buf[0], user_command, strlen(user_command)) == 0)) {
                    // Match found for command, so read and store all attributes.
                    ret = fill_attr_content(node, cmds);
                    if (ret)
                        return -1;
                    break;
                }
            } else {
                fill_content(node, cmds[0]);
                // idx is already incremented in fill_content. So use idx-1.
                if (display_all_commands) {
                    if (!((cmds[0])->num_entries - 1)) {
                        printf("Available vendor commands: \n");
                        printf("\t %-30s \n",
                               (cmds[0])->long_options[(cmds[0])->num_entries - 1].name);
                    } else {
                        printf("\t %-30s \n",
                               (cmds[0])->long_options[(cmds[0])->num_entries - 1].name);
                    }
                }

                if (((cmds[0])->num_entries - 1) == RESERVE_ASCII_VALUE) {
                    memcpy(reservecmd, (cmds[0])->long_options[(cmds[0])->num_entries - 1].name,
                           strlen((cmds[0])->long_options[(cmds[0])->num_entries - 1].name));
                }
            }
        } else if (strncmp(name, "VendorEvent", strlen("VendorEvent")) == 0) {
            // Yet to handle Vendor Event
        } else if (strncmp(name, "VendorRsp", strlen("VendorRsp")) == 0) {
            // Yet to handle Vendor Response
        } else if (strncmp(name, "Attribute", strlen("Attribute")) == 0) {
            // No need to handle attribute tag
        } else {
            printf("Invalid_entry %s\n", name);
        }
    }

    return 0;
}
#endif

#ifdef CONFIG_SUPPORT_VENCMDTABLE
static int parseCommandTable(struct vendor_commands *cmd_ptr, int max_table_idx,
                             struct cmd_params *cmds[], char *user_command)
{
    /* Table index refers to index of command in struture being maintained
     * for set/get commands. cmd_idx refers to index of command in cmds.
     */
    int table_idx = 0, cmd_idx = 0, set_cmd = 0, ret = 0, arg_idx = 0;
    char buf[10] = {0}, response_name[40] = {0};

    struct cmd_params *cmd;

    if (!user_command) {
        cmd = cmds[0];
        /* table_idx and cmd_ptr are referencing the same vendor_commands
         * structure. So, "table_idx <= max_table_idx" check will protect
         * out-of-bounds accesses
         */
        for (table_idx = 0; table_idx <= max_table_idx; table_idx++) {
            cmd_idx = cmd->num_entries;

            cmd->entry[cmd_idx].xml_entry = getCmdType("VendorCmd");

            if (cmd_ptr->cmd_name) {
                cmd->long_options[cmd_idx].name =
                    (char *)malloc(strlen((const char *)cmd_ptr->cmd_name) + 1);
                if (!cmd->long_options[cmd_idx].name) {
                    printf("Failed to allocate memory for name\n");
                }
                memcpy((char *)cmd->long_options[cmd_idx].name,
                       cmd_ptr->cmd_name,
                       strlen((const char *)cmd_ptr->cmd_name) + 1);
                cmd->long_options[cmd_idx].val = cmd_idx;
            }

            if (cmd_ptr->cmd_id) {
                cmd->attr_id[cmd_idx] = cmd_ptr->cmd_id;
            }

            parseDefaultAttrList(&cmd->default_val[cmd_idx],
                                 (const char *)("17,18"));

            if (display_all_commands) {
                if (!cmd_idx) {
                    printf(" Available vendor commands: \n");
                    printf("\t %-30s \n",
                            cmd->long_options[cmd_idx].name);
                } else {
                    printf("\t %-30s \n",
                            cmd->long_options[cmd_idx].name);
                }
            }

            if (cmd_idx == RESERVE_ASCII_VALUE) {
                memcpy(reservecmd, cmd->long_options[cmd_idx].name,
                       strlen(cmd->long_options[cmd_idx].name));
            }

            cmd->num_entries++;
            cmd_ptr++;
        }
    } else {
        for (table_idx = 0; table_idx <= max_table_idx; table_idx++) {
            if (!cmd_ptr->cmd_name)
                break;
            if ((strlen(user_command) == strlen(cmd_ptr->cmd_name)) &&
                (strncmp(user_command, cmd_ptr->cmd_name, strlen(user_command)) == 0)) {
                // Fill command data first
                cmd = cmds[0];
                cmd_idx = cmd->num_entries;

                cmd->entry[cmd_idx].xml_entry = getCmdType("VendorCmd");

                if (cmd_ptr->cmd_name) {
                    cmd->long_options[cmd_idx].name =
                        (char *)malloc(strlen((const char *)cmd_ptr->cmd_name) + 1);
                    if (!cmd->long_options[cmd_idx].name) {
                        printf("Failed to allocate memory for name\n");
                    }
                    memcpy((char *)cmd->long_options[cmd_idx].name,
                            cmd_ptr->cmd_name,
                            strlen((const char *)cmd_ptr->cmd_name) + 1);
                    cmd->long_options[cmd_idx].val = cmd_idx;
                }
                if (cmd_ptr->cmd_id) {
                    cmd->attr_id[cmd_idx] = cmd_ptr->cmd_id;

                    if (cmd->attr_id[cmd_idx] == SET_PARAM){
                        set_cmd = 1;
                    } else if (cmd->attr_id[cmd_idx] == GET_PARAM) {
                        set_cmd = -1;
                        if (cmd_ptr->cmd_name) {
                            memcpy(response_name, cmd_ptr->cmd_name,
                                    strlen(cmd_ptr->cmd_name) + 1);
                        }
                    }
                }

                parseDefaultAttrList(&cmd->default_val[cmd_idx],
                                     (const char *)("17,18"));

                cmd_idx++;
                cmd->num_entries++;

                // After command data, fill attrivute data
                cmd->entry[cmd_idx].xml_entry = getCmdType("Attribute");

                cmd->long_options[cmd_idx].name =
                    (char *)malloc(strlen((const char *)("dparam0")) + 1);
                memcpy((char *)cmd->long_options[cmd_idx].name, "dparam0",
                       strlen("dparam0") + 1);
                cmd->long_options[cmd_idx].val = cmd_idx;

                cmd->attr_id[cmd_idx] = 17;

                cmd->data_type[cmd_idx] = getAttrTypeenum("u32");
                if (cmd->data_type[cmd_idx] >= U8 &&
                    cmd->data_type[cmd_idx] <= BLOB) {
                    cmd->long_options[cmd_idx].has_arg = 1;
                }

                cmd->default_val[cmd_idx].val =
                    (char *)malloc(strlen((const char *)("200")) + 1);
                if (!cmd->default_val[cmd_idx].val) {
                    printf("Failed to allocate memory for default val\n");
                }
                memcpy(cmd->default_val[cmd_idx].val, "200",
                       strlen((const char *)("200")) + 1);

                cmd_idx++;
                cmd->num_entries++;

                cmd->entry[cmd_idx].xml_entry = getCmdType("Attribute");

                cmd->long_options[cmd_idx].name =
                    (char *)malloc(strlen((const char *)("dparam1")) + 1);
                memcpy((char *)cmd->long_options[cmd_idx].name, "dparam1",
                       strlen("dparam1") + 1);
                cmd->long_options[cmd_idx].val = cmd_idx;

                cmd->attr_id[cmd_idx] = 18;

                cmd->data_type[cmd_idx] = getAttrTypeenum("u32");
                if (cmd->data_type[cmd_idx] >= U8 &&
                    cmd->data_type[cmd_idx] <= BLOB) {
                    cmd->long_options[cmd_idx].has_arg = 1;
                }

                snprintf(buf, 10, "%d", cmd_ptr->cmd_value);
                cmd->default_val[cmd_idx].val =
                    (char *)malloc(strlen(buf) + 1);
                if (!cmd->default_val[cmd_idx].val) {
                    printf("Failed to allocate memory for default val\n");
                }
                memcpy(cmd->default_val[cmd_idx].val, buf,
                       strlen(buf) + 1);

                cmd_idx++;
                cmd->num_entries++;

                for (arg_idx = 0; arg_idx < cmd_ptr->n_args; arg_idx++) {
                    cmd->entry[cmd_idx].xml_entry = getCmdType("Attribute");

                    snprintf(buf, 10, "value%d", arg_idx);
                    cmd->long_options[cmd_idx].name =
                        (char *)malloc(strlen((const char *)buf) + 1);
                    memcpy((char *)cmd->long_options[cmd_idx].name, buf,
                            strlen(buf) + 1);
                    cmd->long_options[cmd_idx].val = cmd_idx;

                    cmd->attr_id[cmd_idx] = 19 + arg_idx;

                    cmd->data_type[cmd_idx] = getAttrTypeenum("u32");
                    if (cmd->data_type[cmd_idx] >= U8 &&
                            cmd->data_type[cmd_idx] <= BLOB) {
                        cmd->long_options[cmd_idx].has_arg = 1;
                    }

                    cmd_idx++;
                    cmd->num_entries++;
                }
                if (set_cmd < 0) {
                    // Filling response information first
                    cmd = cmds[1];
                    cmd_idx = 0;
                    cmd->entry[cmd_idx].xml_entry = getCmdType("VendorRsp");

                    cmd->long_options[cmd_idx].name =
                        (char *)malloc(strlen((const char *)response_name) + 1);
                    memcpy((char *)cmd->long_options[cmd_idx].name, response_name,
                            strlen(response_name) + 1);
                    cmd->long_options[cmd_idx].val = cmd_idx;

                    cmd->attr_id[cmd_idx] = 75;

                    cmd->attr_max[cmd_idx] = 2;

                    cmd_idx++;
                    cmd->num_entries++;

                    // Filling corresponding attributes
                    cmd->entry[cmd_idx].xml_entry = getCmdType("Attribute");

                    cmd->long_options[cmd_idx].name =
                        (char *)malloc(strlen((const char *)response_name) + 1);
                    memcpy((char *)cmd->long_options[cmd_idx].name, response_name,
                            strlen(response_name) + 1);
                    cmd->long_options[cmd_idx].val = cmd_idx;

                    cmd->attr_id[cmd_idx] = 1;

                    cmd->data_type[cmd_idx] = getAttrTypeenum("s32");
                    if (cmd->data_type[cmd_idx] >= U8 &&
                            cmd->data_type[cmd_idx] <= BLOB) {
                        cmd->long_options[cmd_idx].has_arg = 1;
                    }

                    cmd_idx++;
                    cmd->num_entries++;
                }
                break;
            } else {
                cmd_ptr++;
            }
        }
    }

    return ret;
}
#endif

#ifdef CONFIG_SUPPORT_LIBROXML
static status parseRoXML(common_data *data, struct cmd_params *cmds[], int num,
                         char *user_command)
{
    struct cmd_params *cmd;
    node_t *doc_ptr, *root;
    char name[MAX_NAME_LEN] = {0};
    int fd = -1;
    size_t filesize = 0;
    char *mmappedData = NULL;
#ifdef CONFIG_SUPPORT_VENCMDTABLE
    struct vendor_commands *ven_cmd_ptr;
    int max_table_idx = 0;
#endif

    if (num < 2)
        return INVALID_ARG;

    cmd = cmds[0];

    filesize = getFilesize(data->config_file);
    if (!filesize)
        return INVALID_ARG;

    fd = open(data->config_file, O_RDONLY, 0);

#ifdef CONFIG_SUPPORT_VENCMDTABLE
    if (data->is_vap_command) {
        ven_cmd_ptr = &vap_vendor_cmds[0];
        max_table_idx = (sizeof(vap_vendor_cmds)/sizeof(vap_vendor_cmds[0])) - 1;
    } else {
        ven_cmd_ptr = &radio_vendor_cmds[0];
        max_table_idx = (sizeof(radio_vendor_cmds)/sizeof(radio_vendor_cmds[0])) - 1;
    }
#endif

    // Get root node for xml doc
    mmappedData = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);

    doc_ptr = roxml_load_buf(mmappedData);
    if (!doc_ptr)
        return INVALID_ARG;

    // Get main xml node
    root = roxml_get_chld(doc_ptr, NULL, 0);
    roxml_get_name(root, &name[0], MAX_NAME_LEN);

    if (strlen(name) == 0) {
        roxml_close(doc_ptr);
        return INVALID_ARG;
    }

    if (strncmp(name, "WCN_VenTool", strlen("WCN_VenTool"))) {
        printf("%s is not a Ven xml\n", data->config_file);
        roxml_close(doc_ptr);
        return INVALID_ARG;
    }

    populateCLIOptions(cmd);

    /* First parse through xml before going into command table*/
    parseRoXMLCommands(root, cmds, user_command);

#ifdef CONFIG_SUPPORT_VENCMDTABLE
    if (!data->skip_cmd_table) {
        parseCommandTable(ven_cmd_ptr, max_table_idx, cmds, user_command);
    }
#endif

    cmd->long_options[cmd->num_entries].name    = NULL;
    cmd->long_options[cmd->num_entries].has_arg = 0;
    cmd->long_options[cmd->num_entries].flag    = 0;
    cmd->long_options[cmd->num_entries].name    = 0;

    roxml_close(doc_ptr);
    munmap(mmappedData, filesize);
    close(fd);
    return SUCCESS;
}

#else
/*
 * This function handles collecting all supported commands and
 * also attributes of the command given by user if there's a match.
 */

int parseXMLCommands(xmlTextReaderPtr reader, struct cmd_params *cmds[],
                     char* user_command, int print_commands)
{
    const xmlChar *name = NULL, *value = NULL;
    int ret, i = 0;
    struct cmd_params *cmd;

    /* Advance reader to first element */
    ret = xmlTextReaderRead(reader);
    name = xmlTextReaderConstName(reader);

#ifdef DEBUG
    printf("Usercmd: %d\n", !!user_command);
#endif
    /* Loop over entire document until EOF */
    while ( ret == 1 ) {
        /* Start with reading only elements (commands/response)
         * instead of all lines.
         */
        if (xmlTextReaderNodeType(reader) == NODE_TYPE_ELEMENT)  {
            name = xmlTextReaderConstName(reader);

            if (name == NULL)
                return -1;

            if (xmlTextReaderNodeType(reader) ==
                    NODE_TYPE_END_ELEMENT ||
                getCmdType((char *)name) == INVALID_CMD) {
                /* Skip copying the empty closing tag
                 * of an entry and invalid commands
                 */
                ret = xmlTextReaderRead(reader);
                continue;
            }
            /* Proceed only if it is a vendor command */
            if (xmlStrcmp(name, (const xmlChar *) "VendorCmd") == 0
#ifdef SUPPORT_VENDOR_EVENT
                || xmlStrcmp(name, (const xmlChar *) "VendorEvent") == 0
#endif
                ) {
                /* This will be executed in second iteration,
                 * when we are searching for user command.
                 */
                if (user_command) {
                    /* Checking for command match */
                    if (xmlStrcmp((const xmlChar *) user_command,
                                  xmlTextReaderGetAttribute(reader,
                                         (const xmlChar *)"name")) == 0) {
                        /* Match found for given command.
                         * So store all attributes/responses.
                         */
                        /* Varible decides where to store attributes.
                         * 0 for command, 1 for response
                         */
                        int cmd_index = 0, cmd_read_done = 0;
                        /*Loop over until all attribute/responses are stored*/
                        do {
                             if (xmlStrcmp(name,
                                     (const xmlChar *) "WCN_VenTool") == 0)
                                 break;
                             if (xmlTextReaderNodeType(reader) !=
                                        NODE_TYPE_ELEMENT ||
                                 xmlTextReaderNodeType(reader) ==
                                        NODE_TYPE_END_ELEMENT ||
                                 getCmdType((char *)name) == INVALID_CMD) {
                                /* Skip copying the empty closing tag
                                 * of an entry and invalid commands
                                 */
                                ret = xmlTextReaderRead(reader);
                                name = xmlTextReaderConstName(reader);
                                if (ret != 1 || !name)
                                    break;
                                continue;
                            }
                            if (xmlStrcmp(name,
                                (const xmlChar *) "VendorCmd") == 0) {
                                /* break condition when we enconter next
                                 * vendor command after a response
                                 */
                                if (cmd_read_done)
                                    break;
                                cmd_read_done = 1;
                            } else if (xmlStrcmp(name,
                                        (const xmlChar *) "VendorRsp") == 0) {
                                cmd_index = 1;
                                cmd_read_done = 1;
#ifdef SUPPORT_VENDOR_EVENT
                            } else if (xmlStrcmp(name,
                                        (const xmlChar *) "VendorEvent") == 0) {
                                cmd_index = 2;
                                cmd_read_done = 1;
#endif
                            }

                            /* cmds has structures for commands, responses and
                             * events in it. cmd_index decides where to
                             * store the attributes.
                             */
                            cmd = cmds[cmd_index];
                            if(!cmd) {
                                printf("cmd is NULL\n");
                                return -1;
                            }
                            i = cmd->num_entries;

                            cmd->entry[i].xml_entry = getCmdType((char *)name);

                            /* Storing attribute's name */
                            value = xmlTextReaderGetAttribute(reader,
                                        (const xmlChar *)"name");
                            if(value) {
                                cmd->long_options[i].name =
                                    (char *)malloc(strlen((const char *)value)
                                                   + 1);
                                if (!cmd->long_options[i].name) {
                                    printf("Failed to allocate memory"
                                            " for name\n");
                                }
                                memcpy((char *)cmd->long_options[i].name,
                                       value, strlen((const char *)value)+1);
                                cmd->long_options[i].val = i;
                            }

                            /* Storing attribute's ID */
                            value = xmlTextReaderGetAttribute(reader,
                                        (const xmlChar *) "ID");
                            if(value){
                                cmd->attr_id[i] = atoi((const char *)value);
                            }

                            /*
                             * Store switch case value if present. Either ID
                             * or case will be present in the XML entry
                             */
                            value = xmlTextReaderGetAttribute(reader,
                                        (const xmlChar *) "value");
                            if(value) {
                                cmd->attr_id[i] = atoi((const char *)value);
                            }

                            /* Storing attribute's type */
                            value = xmlTextReaderGetAttribute(reader,
                                        (const xmlChar *)"TYPE");
                            if(value) {
                                cmd->data_type[i] =
                                    getAttrTypeenum((char *)value);
                                if (cmd->data_type[i] >= U8 &&
                                    cmd->data_type[i] <= BLOB)
                                    cmd->long_options[i].has_arg = 1;
                            }

                            /* Storing vendor command's default value */
                            value = xmlTextReaderGetAttribute(reader,
                                        (const xmlChar *) "DEFAULTS");
                            if(value) {
                                parseDefaultAttrList(&cmd->default_val[i],
                                                     (const char *)value);
                            }

                            /* Storing attribute's default value */
                            value = xmlTextReaderGetAttribute(reader,
                                        (const xmlChar *)"DEFAULT");
                            if(value) {
                                cmd->default_val[i].val =
                                    (char *)malloc(strlen((const char *)value)
                                                          + 1);
                                if (!cmd->default_val[i].val) {
                                    printf("Failed to allocate memory"
                                            " for default val\n");
                                }
                                memcpy((char *)cmd->default_val[i].val, value,
                                       strlen((const char *)value)+1);
                            }

                            /* Storing attribute's maximum attribute count */
                            value = xmlTextReaderGetAttribute(reader,
                                        (const xmlChar *) "ATTR_MAX");
                            if(value) {
                                cmd->attr_max[i] = atoi((const char *)value);
                            }

#ifdef DEBUG
                            printf("num_entries: %d, xml_entry: type: %s index: %d attr_id: %d name: %s\n",
                                   cmd->num_entries, name,
                                   cmd->entry[i].xml_entry, cmd->attr_id[i],
                                   cmd->long_options[i].name);
#endif
                            /* Read next line */
                            xmlTextReaderRead(reader);
                            if (!reader)
                                break;
                            name = xmlTextReaderConstName(reader);
                            /* Entry has been read but the content/name of the
                             * entry is NULL. Exit reading the file
                             */
                            if (!name)
                                break;
                            cmd->num_entries++;

                        } while (1);
                        /* Break out of outer while loop as well
                         * we've read everything required
                         */
                        ret = 0;
                        break;
                    }
                } else {
                    /* Excecuted in first iteration where we read
                     * vendor commands only.
                     */
                    cmd = cmds[0];
                    i = cmd->num_entries;

                    cmd->entry[i].xml_entry = getCmdType((char *)name);

                    /* Storing attribute's name */
                    value = xmlTextReaderGetAttribute(reader,
                                (const xmlChar *) "name");
                    if(value) {
                        cmd->long_options[i].name =
                                (char *)malloc(strlen((const char *)value)+1);
                        if (!cmd->long_options[i].name) {
                            printf("Failed to allocate memory for name\n");
                        }
                        memcpy((char *)cmd->long_options[i].name, value,
                                strlen((const char *)value)+1);
                        cmd->long_options[i].val = i;
                    }

                    /* Storing attribute's ID */
                    value = xmlTextReaderGetAttribute(reader,
                                (const xmlChar *) "ID");
                    if(value) {
                        cmd->attr_id[i] = atoi((const char *)value);
                    }

                    /*
                     * Store switch case value if present. Either ID
                     * or case will be present in the XML entry
                     */
                    value = xmlTextReaderGetAttribute(reader,
                                (const xmlChar *) "value");
                    if(value) {
                        cmd->attr_id[i] = atoi((const char *)value);
                    }

                    /* Storing attribute's type */
                    value = xmlTextReaderGetAttribute(reader,
                                (const xmlChar *) "TYPE");
                    if(value) {
                        cmd->data_type[i] = getAttrTypeenum((char *)value);
                        if (cmd->data_type[i] >= U8 &&
                            cmd->data_type[i] <= BLOB)
                            cmd->long_options[i].has_arg = 1;
                    }

                    /* Storing vendor command's default value */
                    value = xmlTextReaderGetAttribute(reader,
                                (const xmlChar *) "DEFAULTS");
                    if(value) {
                        parseDefaultAttrList(&cmd->default_val[i],
                                (const char *)value);
                    }

                    /* Storing attribute's default value */
                    value = xmlTextReaderGetAttribute(reader,
                                (const xmlChar *) "DEFAULT");
                    if(value) {
                        cmd->default_val[i].val =
                                (char *)malloc(strlen((const char *)value)+1);
                        if (!cmd->default_val[i].val) {
                            printf("Failed to allocate memory"
                                   " for default val\n");
                        }
                        memcpy((char *)cmd->default_val[i].val, value,
                               strlen((const char *)value)+1);
                    }

                    /* Storing attribute's maximum attribute count */
                    value = xmlTextReaderGetAttribute(reader,
                                (const xmlChar *) "ATTR_MAX");
                    if(value) {
                        cmd->attr_max[i] = atoi((const char *)value);
                    }
#if defined(__IPQ__) || defined(SDX_MCC)
                    if (print_commands && display_all_commands) {
                        if (!i) {
                            printf(" Available vendor commands: \n");
                            printf("\t %-30s \n",
                                    cmd->long_options[i].name);
                        } else {
                            printf("\t %-30s \n",
                                    cmd->long_options[i].name);
                        }
                    }

                    if ((print_commands &&
                                (cmd->num_entries == RESERVE_ASCII_VALUE))) {
                        memcpy(reservecmd, cmd->long_options[i].name,
                                strlen(cmd->long_options[i].name));
                    }
#endif /* __IPQ__ || SDX_MCC */
#ifdef DEBUG
                    printf("num_entries: %d, xml_entry: type: %s index: %d attr_id: %d name: %s\n",
                           cmd->num_entries, name,
                           cmd->entry[i].xml_entry, cmd->attr_id[i],
                           cmd->long_options[i].name);
#endif
                    cmd->num_entries++;
                }
            }
        }
        ret = xmlTextReaderRead(reader); /* move to next node */
    }
    return 0;
}

/* ParseXMLDocReader is called twice in this file.
 * First time is to store all the commands and print
 * if no command argument is given. Second itearation
 * is to compare with user input to find a match.
 */

static status parseXMLDocReader(common_data *data, struct cmd_params *cmds[],
        int num, char *user_command)
{
    struct cmd_params *cmd;
    xmlTextReaderPtr reader;
    const xmlChar *name = NULL;
    char *config_file = data->config_file;
#ifdef CONFIG_SUPPORT_VENCMDTABLE
    struct vendor_commands *ven_cmd_ptr;
    int max_table_idx = 0;
#endif

    if (num < 2)
        return INVALID_ARG;
    cmd = cmds[0];

    /* Stream the file with no blanks flag to discard null nodes
     * in xml automatically.
     */
    reader = xmlReaderForFile(config_file, "UTF-8",
                              XML_PARSE_RECOVER | XML_PARSE_NOBLANKS);
    if (reader == NULL)
        return INVALID_ARG;

#ifdef CONFIG_SUPPORT_VENCMDTABLE
    if (data->is_vap_command) {
        ven_cmd_ptr = &vap_vendor_cmds[0];
        max_table_idx = (sizeof(vap_vendor_cmds)/sizeof(vap_vendor_cmds[0])) - 1;
    } else {
        ven_cmd_ptr = &radio_vendor_cmds[0];
        max_table_idx = (sizeof(radio_vendor_cmds)/sizeof(radio_vendor_cmds[0])) - 1;
    }
#endif

    /* Skip comments and read until first node */
    do {
        xmlTextReaderRead(reader);
    } while(xmlTextReaderNodeType(reader) == NODE_TYPE_COMMENT);

    name = xmlTextReaderConstName(reader);
    if (name == NULL)
        return INVALID_ARG;

    if(xmlStrcmp(name, (const xmlChar *) "WCN_VenTool")) {
        printf("%s not a Ven xml\n", config_file);
        xmlFreeTextReader(reader);
        return INVALID_ARG;
    }

    populateCLIOptions(cmd);

    /* Populate all commands & elments to internal data structure */
    parseXMLCommands(reader, cmds, user_command, 1);

#ifdef CONFIG_SUPPORT_VENCMDTABLE
    //Append local cmds here
    if (!data->skip_cmd_table) {
        parseCommandTable(ven_cmd_ptr, max_table_idx, cmds, user_command);
    }
#endif

    cmd->long_options[cmd->num_entries].name = NULL;
    cmd->long_options[cmd->num_entries].has_arg = 0;
    cmd->long_options[cmd->num_entries].flag = 0;
    cmd->long_options[cmd->num_entries].val = 0;

    xmlFreeTextReader(reader);
    return SUCCESS;
}

#ifdef SUPPORT_VENDOR_EVENT
/* Parse event inputs;
 * This API expects only long options as described in the
 * xml file. Any inputs out of xml file will be discarded.
 */
static void parseEvtInputs(struct nlIfaceInfo *info, int argc, char **argv,
                           struct cmd_params *evt, int *option_index,
                           common_data data)
{
    int c;
    struct cmd_params command, response, event;
    struct cmd_params *cur_cmds[NUM_NLMSG_TYPES] =
                                    {&command, &response, &event};
    struct resp_event_info event_info;

    memset(&command, 0, sizeof(struct cmd_params));
    memset(&response, 0, sizeof(struct cmd_params));
    memset(&event, 0, sizeof(struct cmd_params));
    memset(&event_info, 0, sizeof(struct resp_event_info));

    /* Parse for event id; This is the first input in the command params */
    c = getopt_long(argc, argv, short_options,
                    evt->long_options, option_index);
    if (c == -1 || c > MAX_OPTIONS-1)
        return;
    /*
     * for ASCII(?) i.e 63rd entry also we will hit below condition
     * we need to differentiate between actual command not find.
     */
    if (c == '?') {
#ifdef __WIN__
        if (strncmp(reservecmd,  &(argv[ARGV_COMMAND_OFFSET][2]),
                    strlen(&(argv[ARGV_COMMAND_OFFSET][2]))) != 0) {
            printf("%s:%d Unsupported Event\n",__func__,__LINE__);
            return;
        }
#endif
    }

    if (c > evt->num_entries || evt->entry[c].xml_entry != VEN_EVENT) {
        printf("Event not present: c = %d, entry = %d\n",
               c, evt->entry[c].xml_entry);
        return;
    }

    if (parseXMLDocReader(&data, cur_cmds, NUM_NLMSG_TYPES,
                          (char *)evt->long_options[c].name) != SUCCESS) {
        printf ("Failed to parse the file : %s\n", argv[1]);
        return;
    }

    if (evt->attr_id[c]) {
        event_info.rsp = &event;
#ifndef __WIN__
        printf("Monitor for Vendor Event: %d\n", evt->attr_id[c]);
#endif
        startMonitorForEvent(info, &event_info, evt->attr_id[c]);
    }
}
#endif /* SUPPORT_VENDOR_EVENT */
#endif /* CONFIG_SUPPORT_LIBROXML */

static int isRawDataVendorCmd(int command_id)
{
    switch (command_id) {
    case QCA_NL80211_VENDOR_SUBCMD_GET_BUS_SIZE:
    case QCA_NL80211_VENDOR_SUBCMD_GET_RADAR_HISTORY:
    case QCA_NL80211_VENDOR_SUBCMD_DFS_CAPABILITY:
    case QCA_NL80211_VENDOR_SUBCMD_STATS_EXT:
    case QCA_NL80211_VENDOR_SUBCMD_GET_SUPPORTED_FEATURES:
    case QCA_NL80211_VENDOR_SUBCMD_SCANNING_MAC_OUI:
    case QCA_NL80211_VENDOR_SUBCMD_GET_FEATURES:
    case QCA_NL80211_VENDOR_SUBCMD_KEY_MGMT_SET_KEY:
    case QCA_NL80211_VENDOR_SUBCMD_TDLS_GET_CAPABILITIES:
    case QCA_NL80211_VENDOR_SUBCMD_AVOID_FREQUENCY:
    case QCA_NL80211_VENDOR_SUBCMD_GET_WAKE_REASON_STATS:
    case QCA_NL80211_VENDOR_SUBCMD_GETBAND:
    case QCA_NL80211_VENDOR_SUBCMD_NUD_STATS_GET:
        return 1;
    default:
        return 0;
    }
}
/* Parse command inputs;
 * This API expects only long options as described in the
 * xml file. Any inputs out of xml file will be discarded.
 */
static int parseCmdinputs(struct nlIfaceInfo *info, int argc, char **argv,
                    struct cmd_params *cmd, int *option_index,
                    common_data data)
{
    int c, ret = 0, is_raw_data = 0;
    struct nl_msg *nlmsg;
    struct nlattr *nlVenData = NULL;
    struct cmd_params command, response, event;
    struct cmd_params *cur_cmds[NUM_NLMSG_TYPES] =
                                    {&command, &response, &event};
    struct resp_event_info resp_info;

    memset(&command, 0, sizeof(struct cmd_params));
    memset(&response, 0, sizeof(struct cmd_params));
    memset(&event, 0, sizeof(struct cmd_params));
    memset(&resp_info, 0, sizeof(struct resp_event_info));

    /* Use this response attributes while parsing the response */
    resp_info.rsp = &response;

    /* Parse for command id; This is the first input in the command params */
    c = getopt_long(argc, argv, short_options,
            cmd->long_options, option_index);
    if (c == -1 || c > MAX_OPTIONS-1)
        return -ENOTSUP;
    /*
     * for ASCII(?) i.e 63rd entry also we will hit below condition
     * we need to differentiate between actual command not find.
     */
    if (c == '?') {
#if defined(__IPQ__) || defined(SDX_MCC)
        if (strncmp(reservecmd,  &(argv[ARGV_COMMAND_OFFSET][2]),
                    strlen(&(argv[ARGV_COMMAND_OFFSET][2]))) != 0) {
            printf("%s:%d Unsupported Command \n",__func__,__LINE__);
            return -ENOTSUP;
        }
#endif /* __IPQ__ || SDX_MCC */
    }

    if (c > cmd->num_entries || cmd->entry[c].xml_entry != VEN_CMD) {
        printf("Command not present: c = %d, entry = %d\n",
                c, cmd->entry[c].xml_entry);
        return -ENOTSUP;
    }

#ifdef CONFIG_SUPPORT_LIBROXML
    if (parseRoXML(&data, cur_cmds, NUM_NLMSG_TYPES,
                   (char *)cmd->long_options[c].name) != SUCCESS) {
        printf("Failed to parse the file : %s\n", argv[1]);
        return -ENOTSUP;
    }
#else
    if (parseXMLDocReader(&data, cur_cmds, NUM_NLMSG_TYPES,
                          (char *)cmd->long_options[c].name) != SUCCESS) {
        printf ("Failed to parse the file : %s\n", argv[1]);
        return -ENOTSUP;
    }
#endif

    c = O_CMD_LAST-1;

    /* command.attr_id[c] carries the vendor command number */
    nlmsg = prepareNLMsg(info, command.attr_id[c], data.iface);
    if (!nlmsg) {
        printf("Failed to prepare nlmsg\n");
        return -ENOTSUP;
    }

    is_raw_data = isRawDataVendorCmd(command.attr_id[c]);
    if (is_raw_data) {
        ret = nla_put(nlmsg, NL80211_ATTR_VENDOR_DATA, 0, NULL);
        if (ret)
            return ret;
    } else {
        nlVenData = startVendorData(nlmsg);
        if (!nlVenData)
            printf("failed to start vendor data\n");
    }

#ifdef __IPQ__
    if (data.link_id != MLO_INVALID_LINK_ID) {
        if (command.attr_id[c] == QCA_NL80211_VENDOR_SUBCMD_SET_WIFI_CONFIGURATION ||
            command.attr_id[c] == QCA_NL80211_VENDOR_SUBCMD_GET_WIFI_CONFIGURATION) {
            if (nla_put(nlmsg, QCA_WLAN_VENDOR_ATTR_CONFIG_MLO_LINK_ID,
                        sizeof(uint8_t), &(data.link_id)))
                return MEM_NOT_AVAILABLE;
        }
    }
#endif

    populateDefaultAttrs(argc, argv, &command, option_index, &c, nlmsg, 0);

    memcpy(&(resp_info.rsp)->iface[0], &data.iface[0], IFACE_LEN);

    /* Parse for each input and populate it accordingly */
    while (1) {
        c = getopt_long(argc, argv, short_options,
                command.long_options, option_index);
        if (c == -1 || c > MAX_OPTIONS-1)
            break;
        if (command.entry[c].xml_entry == ATTRIBUTE) {
            if (is_raw_data ) {
                printf("Attributes are not expcted with raw_data\n");
                break;
            }
            if (populateAttribute(argc, argv, &command,
                option_index, &c, nlmsg, 0) != 0) {
                printf("Failed to fill Attributes\n");
                break;
            }
        } else if (command.entry[c].ctrl_option == O_END_CMD) {
            if (nlVenData)
                endVendorData(nlmsg, nlVenData);
            ret = sendNLMsg(info, nlmsg, &resp_info);
            if (ret != 0) {
                printf("Failed to send message to driver Error:%d\n", ret);
                break;
            }
        } else if (command.entry[c].ctrl_option == O_RESPONSE) {
            c = getopt_long(argc, argv, short_options,
                    command.long_options, option_index);
            if (c == -1 || c > MAX_OPTIONS-1)
                break;
            if (command.attr_id[c]) {
#if !defined(__IPQ__) && !defined(SDX_MCC)
                printf("Monitor for Response : %d\n", command.attr_id[c]);
#endif /* __IPQ__ || SDX_MCC */
                startmonitorResponse(&resp_info, command.attr_id[c]);
            }
        } else {
            printf("Unknown CLI option\n");
            ret = -ENOTSUP;
        }
    }
    free(resp_info.response);
    return ret;
}

/* General tendency is to provide common data first and then
 * command specific data. This API expects iface and file inputs in
 * any order and then command specific data
 */
static status get_common_data(common_data *data,
                       int argc,
                       char** argv,
                       const char* short_options,
                       struct option *long_options)
{
    int data_read = 0; //No.of elements to be read as common data
    unsigned int arg_len;
#if defined(__IPQ__) || defined(SDX_MCC)
    char help_command[FILE_NAME_LEN];
#endif /* __IPQ__ || SDX_MCC */
    while (1) {
        int option_index = 0, c;

        c = getopt_long(argc, argv, short_options,
                long_options, &option_index);
        if (c == -1)
            break;

        arg_len = optarg ? strlen(optarg) : 0;
        switch(c) {
            case 'i':
                if (!arg_len || arg_len >= IFACE_LEN) {
                    printf("iface name too long or empty\n");
                    return INVALID_ARG;
                }

                memcpy(&data->iface[0], optarg, arg_len);
                data->iface[arg_len] = '\0';
                data_read++;
            break;
            case 'f':
                if (!arg_len || arg_len >= FILE_NAME_LEN) {
                    printf("file name too long or empty\n");
                    return INVALID_ARG;
                }

                memcpy(&data->config_file[0], optarg, arg_len);
                data->config_file[arg_len] = '\0';
                data_read++;
            break;
            case 'h':
#if defined(__IPQ__) || defined(SDX_MCC)
                if (arg_len >= FILE_NAME_LEN)
                    arg_len = FILE_NAME_LEN - 1;

                /*
                 * Copy dest to src with NULL terminator,
                 * or start with terminator if dest invalid.
                 */
                if (arg_len)
                    memcpy(help_command, optarg, arg_len);
                help_command[arg_len] = '\0';

                if (strncmp(help_command, "list", 4) == 0 ) {
                    display_all_commands = 1;
                }
                data_read++;
#endif /* __IPQ__ || SDX_MCC */
            break;
#ifdef __IPQ__
            case 'l':
                if (!arg_len) {
                    printf("link_id is empty\n");
                    return INVALID_ARG;
                }
                if (strncmp(optarg, "none", 4) == 0 ) {
                    /* assign 0xff if link_id not specified */
                    data->link_id = MLO_INVALID_LINK_ID;
                    data_read++;
                    break;
                }

                if ((arg_len > LINK_ID_VALID_LEN) || (atoi(optarg) < 0) ||
                               (atoi(optarg) > MAX_LINK_ID)) {
                     printf("link_id invalid\n");
                     return INVALID_ARG;
                }

                data->link_id = atoi(optarg);
                data_read++;
                break;
#endif
        }
        if (data_read == NO_OF_ELEMENTS_IN_COMMON_DATA) {
            break;
        }
    }
    if (strlen(data->iface) == 0) {
        printf ("Failed to get iface\n");
        return INVALID_ARG;
    }

#ifdef __IPQ__
    if (strcasecmp(data->config_file, "/lib/wifi/qcacommands_mesh.xml") == 0) {
        data->skip_cmd_table = 1;
        data->is_vap_command = 1;
    }
    if (strcasecmp(data->config_file, "/lib/wifi/qcacommands_vap.xml") == 0) {
        data->is_vap_command = 1;
    }
#endif
#ifdef SDX_MCC
    if (strcasecmp(data->config_file, "/etc/misc/wifi/qcacommands_mesh.xml") == 0) {
        data->skip_cmd_table = 1;
        data->is_vap_command = 1;
    }
    if (strcasecmp(data->config_file, "/etc/misc/wifi/qcacommands_vap.xml") == 0) {
        data->is_vap_command = 1;
    }
#endif /* SDX_MCC */
    if (strlen(data->config_file) == 0) {
        printf ("Failed to get input file\n");
        return INVALID_ARG;
    }

    return SUCCESS;
}


static status dissolveLongOptions(struct cmd_params *cmds[], int num)
{
    int i, j;
    struct cmd_params *cmd;

    for (i=0; i<num; i++) {
        cmd = cmds[i];
        for (j=0; j<cmd->num_entries;j++) {
            free((void *)cmd->long_options[j].name);
        }
    }

    return SUCCESS;
}


int main(int argc, char **argv)
{
    struct nlIfaceInfo *info;
    struct cmd_params command, response, event;
    struct cmd_params *cmds[NUM_NLMSG_TYPES] = {&command, &response, &event};
    common_data data;
    int retval = 0;

    memset(&command, 0, sizeof(struct cmd_params));
    memset(&response, 0, sizeof(struct cmd_params));
    memset(&event, 0, sizeof(struct cmd_params));

    memset(&data, 0, sizeof(common_data));
#ifdef __IPQ__
    data.link_id = MLO_INVALID_LINK_ID;
#endif

    if (get_common_data(&data, argc, argv, short_options,
                        cmds[NLMSG_TYPE_COMMAND]->long_options) != SUCCESS) {
        printf ("Failed to get common data\n");
        return INVALID_ARG;
    }

#ifdef CONFIG_SUPPORT_LIBROXML
    if (parseRoXML(&data, cmds, NUM_NLMSG_TYPES, NULL)
            != SUCCESS) {
        printf ("Failed to parse the file : %s\n", argv[1]);
        return INVALID_ARG;
    }
#else
    if (parseXMLDocReader(&data, cmds, NUM_NLMSG_TYPES, NULL)
            != SUCCESS) {
        printf ("Failed to parse the file : %s\n", argv[1]);
        return INVALID_ARG;
    }
#endif

    info = NCT_initialize();
    if (info == NULL) {
        printf ("Failed to initialize sockets\n");
        return INVALID_ARG;
    }

    memcpy(info->ifname, data.iface, IFACE_LEN);

    while (1) {
        int option_index = 0, c;

        /* Parse for command/event params from CLI */
        c = getopt_long(argc, argv, short_options,
                cmds[NLMSG_TYPE_COMMAND]->long_options, &option_index);
        if (c == -1 || c > MAX_OPTIONS-1)
            break;

        if (c < cmds[NLMSG_TYPE_COMMAND]->num_entries) {
            if (cmds[NLMSG_TYPE_COMMAND]->entry[c].ctrl_option == O_COMMAND) {
                retval = parseCmdinputs(info, argc, argv, cmds[NLMSG_TYPE_COMMAND],
                               &option_index, data);
#ifndef CONFIG_SUPPORT_LIBROXML
#ifdef SUPPORT_VENDOR_EVENT
            } else if (cmds[NLMSG_TYPE_COMMAND]->entry[c].ctrl_option == O_EVENT) {
                parseEvtInputs(info, argc, argv, cmds[NLMSG_TYPE_COMMAND],
                               &option_index, data);
#endif
#endif
            } else {
                printf("Unknown Command : %d\n",
                       cmds[NLMSG_TYPE_COMMAND]->entry[c].ctrl_option);
                retval = -ENOTSUP;
            }
        } else
            printf("getopt returned character code %d \n", c);
    }
    dissolveLongOptions(cmds, 3);
    cleanupNLinfo(info);
    return retval;
}
