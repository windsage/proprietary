"""
    Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries
    All rights reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
"""

import json
import struct
import os
import sys

global banner
global datatype_size
global formatter

banner = "/*\n * THIS IS AUTO GENERATED FILE.\n * DO NOT ALTER MANUALLY\n */\n"
datatype_size = {"uint8_t" : 1, "uint16_t" : 2, "uint32_t" : 4, "uint64_t": 8}
formatter = {"uint8_t" : 'B', "uint16_t" : 'H', "uint32_t" : 'I', "uint64_t": 'Q'}

def read_json(json_file):

	with open(json_file) as f:
		data = json.load(f)
	return data

def generate_copyright(json_data, header_file, copyright_file):

	for line in copyright_file:
		header_file.write(line)

def generate_header_file(json_data, header_file):

	header_file.write('\n')
	for file in json_data["HEADER"]:
		header_file.write(f'#include <{file}>\n')
	header_file.write('\n')

def generate_macro(json_data, header_file):

	for macro, value in json_data["MACRO"].items():
		header_file.write(f'#define {macro} {value}\n')
	header_file.write('\n')

def generate_version(json_data, header_file):

	for macro,version in json_data["VERSION"].items():
		header_file.write(f'#define {macro} {version : #010x}\n')

def generate_event_enums(json_data, header_file):

	header_file.write('\nenum qca_chipset_stats_event_type {\n')
	first_event = 0
	for event, structure in list(json_data['EVENTS'].items()):
		if (first_event == 0):
			header_file.write(f'\t{event} = {0},\n')
			first_event = 1
			continue
		header_file.write(f'\t{event},\n')
	header_file.write('};\n')

def generate_enum(json_data, header_file):

	for  key, value in json_data['ENUMS'].items():
		header_file.write('\n' + f'enum {key}' + ' {\n')
		for index in range(len(value['param'])):
			if (index == 0):
				header_file.write(f'\t{value["param"][index]} = {value["init"]},\n')
			else:
				header_file.write(f'\t{value["param"][index]},\n')
		header_file.write('};\n')

def generate_structure(json_data, header_file):

	for structure, values in list(json_data['STRUCTURES'].items()):
		header_file.write('\n' + f'struct {structure}' + ' {')
		for field, data in list(values[0].items()):
			if (data[0] == 'struct'):
				if(len(data) == 2):
					header_file.write('\n\t' + f'{data[0]} {data[1] } {field};')
				else:
					header_file.write('\n\t' + f'{data[0]} {data[1] } {field}[{data[3]}];')
			elif(data[0] == 0):
				if (len(data) == 6):
					header_file.write('\n\t' + f'{data[1]} {field}[{data[3]} + {data[4]}];')
				else:
					-header_file.write('\n\t' + f'{data[1]} {field}[{data[3]}];')
			else:
				if (len(data) == 3):
					header_file.write('\n\t' + f'{data[1]} {field};')
				else:
					header_file.write('\n\t' + f'{data[1]} {field}:{data[2]}')
		header_file.write('\n} qdf_packed;\n')
	structure_set = set()
	for event, structure in list(json_data['EVENTS'].items())[:-1]:
		if (structure[0]["event_info"][0] not in structure_set and structure[0]["event_info"][0] not in json_data['STRUCTURES_NOT_REQUIRED']):
			header_file.write('\n' + f'struct {structure[0]["event_info"][0]}' + ' {')
			structure_set.add(structure[0]["event_info"][0])
		else:
			continue
		for field, data in list(structure[0].items()):
			if (field == 'event_info'):
				continue
			elif (data[0] == 'struct' and data):
				if(len(data) == 2):
					header_file.write('\n\t' + f'{data[0]} {data[1] } {field};')
				else:
					header_file.write('\n\t' + f'{data[0]} {data[1] } {field}[{data[3]}];')
			elif (data[0] == 0):
				if (len(data) == 6):
					header_file.write('\n\t' + f'{data[1]} {field}[{data[3]} + {data[4]}];')
				else:
					header_file.write('\n\t' + f'{data[1]} {field}[{data[3]}];')
			else :
				if (len(data) == 3):
					header_file.write('\n\t' + f'{data[1]} {field};')
				else:
					header_file.write('\n\t' + f'{data[1]} {field}:{data[2]}')
		header_file.write('\n} qdf_packed;\n')

def generate_file(json_data):

	header_file_name = json_data['FILE_NAME']
	header_file = open(header_file_name[0],"w")
	copyright_file = open(json_data['COPYRIGHT_FILE'][0] + '.txt', 'r')
	generate_copyright(json_data, header_file, copyright_file)
	header_file.write('\n')
	header_file.write(banner)
	global_macro = '__' + str(header_file_name[0][:-2]).upper() + '_H'
	header_file.write('\n#ifndef ' + global_macro)
	header_file.write('\n#define ' + global_macro)
	header_file.write('\n')
	generate_header_file(json_data, header_file)
	generate_macro(json_data, header_file)
	generate_version(json_data, header_file)
	generate_event_enums(json_data, header_file)
	generate_enum(json_data, header_file)
	generate_structure(json_data, header_file)
	header_file.write('\n#endif /* ' + global_macro + ' */')
	header_file.close()
	print("Header Generation done")

def get_events(json_data):
	event_list = []
	for event, structure in list(json_data['EVENTS'].items()):
		event_list.append(event)
	return event_list

def read_binary(dump_file):

	decoded_file = dump_file.split('.')[0] + "_decoded.txt"
	print(f"\nLogs stored in {decoded_file}\n")
	decoded_file = open(decoded_file, "w")
	binary_data = open(dump_file, "rb").read()
	return binary_data, decoded_file


def decode_metadata(binary_data, start_marker_ascii, end_marker_ascii):

	start_mkr_finder_flag = 0
	data_index = 0
	metadata = []
	binary_data_list = list(binary_data)
	while (data_index < len(binary_data_list)):
		if (binary_data_list[data_index : data_index + len(start_marker_ascii)] == start_marker_ascii and start_mkr_finder_flag == 0):
			start_mkr_finder_flag = 1
			hdr_start_index = data_index
			continue
		if (start_mkr_finder_flag == 1 and binary_data_list[ data_index : data_index + len(end_marker_ascii)] == end_marker_ascii):
			metadata = metadata + binary_data_list[ data_index : data_index + len(end_marker_ascii)]
			hdr_end_index = data_index + len(end_marker_ascii)
			break
		if (start_mkr_finder_flag == 1):
			metadata.append(binary_data_list[data_index])
		data_index += 1
	metadata_str = ""
	binary_data_list = binary_data_list[hdr_end_index : ]
	for element in metadata:
		metadata_str = metadata_str + chr(element)
	metadata_str = metadata_str.replace('CS_HSM', '').replace('CS_HEM', '')
	hdr_version_idx = metadata_str.find("hdr_version") + len("hdr_version") + 3
	endianness_version_idx = metadata_str.find("Endianness") + len("Endianness") + 3
	drop_cnt_idx = metadata_str.find("Drop cnt") + len("Drop cnt") + 3
	print("hdr_version : ", metadata_str[hdr_version_idx])
	print("endianness_version : ", metadata_str[endianness_version_idx])
	if (metadata_str[endianness_version_idx] == '0'):
		endianness = '<'
	elif (metadata_str[endianness_version_idx] == '1'):
		endianness = '>'
	else:
		print('Invalid endianess')
		exit()
	print("drop_cnt : ", metadata_str[drop_cnt_idx])
	return binary_data_list, metadata_str[hdr_version_idx], endianness, metadata_str[drop_cnt_idx]

def array_reverse(array, array_size, byte):

	index = 0
	while(index < array_size):
		left_idx = index
		right_idx = min(index + byte - 1, array_size - 1)
		while (left_idx < right_idx):
			array[left_idx], array[right_idx] = array[right_idx], array[left_idx]
			left_idx += 1
			right_idx -= 1
		index += byte
	return array

def timestamp_calculator(time):

	millisec = str(time % 1000000)
	time = time // 1000000
	second = str(time % 60)
	time = time // 60
	minute = str(time % 60)
	hour = str(time // 60)
	timestamp = f'{hour.zfill(2)}:{minute.zfill(2)}:{second.zfill(2)}.{millisec}'
	return timestamp

def decode_ssid(data_list, meta_data, index, ssid_len):

	data_size = datatype_size[meta_data[1]] * ssid_len
	payload_data = data_list[index : index + data_size ]
	ssid = ''
	for elem in payload_data:
		ssid += f'{chr(elem)}'
	decoded_file.write(f' {meta_data[-1]} : {ssid},')
	index += meta_data[2]
	return index

def decode_macaddr(data_list, meta_data, data_element_idx):

	data_size = (datatype_size[meta_data[1]] * meta_data[2])
	payload_data = data_list[data_element_idx : data_element_idx + data_size ]
	addr = ''
	for elem in payload_data[:-1]:
		addr +=f'{ hex(elem)[2:]}:'
	addr = addr + '**:**:' + str(payload_data[-1]).zfill(2)
	decoded_file.write(f' {meta_data[-1]} : {addr[:-1]},')
	data_element_idx += data_size
	return data_element_idx

def scanned_freq_decoder(data_list, meta_data, data_element_idx, num_scanned_freq):

	data_size = datatype_size[meta_data[1]] * num_scanned_freq
	payload_data = data_list[data_element_idx : data_element_idx + data_size ]
	payload_data = array_reverse(payload_data, data_size, datatype_size[meta_data[1]])
	decoded_file.write(f' {meta_data[-1]} : {payload_data},')
	data_element_idx += meta_data[2] * datatype_size[meta_data[1]]
	return data_element_idx

def decode_array(data_list, meta_data, data_element_idx, endianness):

	data_size = (datatype_size[meta_data[1]] * meta_data[2])
	payload_data = data_list[data_element_idx : data_element_idx + data_size ]
	if (endianness == '>'):
		ayload_data = array_reverse(payload_data, data_size, datatype_size[meta_data[1]])
	decoded_file.write(f' {meta_data[-1]} : {payload_data},')
	data_element_idx += data_size
	return data_element_idx

def decode_flags(data_list, meta_data, data_element_idx):

	flag_data = bin(data_list[data_element_idx])[2:].zfill(8)
	decoded_file.write(f' HT : {flag_data[-1]},')
	decoded_file.write(f' VHT : {flag_data[-2]},')
	decoded_file.write(f' HE : {flag_data[-3]},')
	decoded_file.write(f' EHT : {flag_data[-4]},')
	data_element_idx += datatype_size[meta_data[1]]
	return data_element_idx

def decode_num_scanned_freq(data_list, meta_data, data_element_idx, endianness):

	data_size = datatype_size[meta_data[1]]
	payload_data = data_list[data_element_idx : data_element_idx + data_size ]
	format_endianness = endianness + formatter[meta_data[1]]
	num_scanned_freq = struct.unpack(format_endianness, bytes(payload_data))[0]
	decoded_file.write(f' {meta_data[-1]} : {num_scanned_freq},')
	data_element_idx += datatype_size[meta_data[1]]
	return data_element_idx, num_scanned_freq

def decode_integer(data_list, meta_data, data_element_idx, endianness):

	data_size = datatype_size[meta_data[1]]
	payload_data = data_list[data_element_idx : data_element_idx + data_size ]
	format_endianness = endianness + formatter[meta_data[1]]
	payload_data = struct.unpack(format_endianness, bytes(payload_data))[0]
	decoded_file.write(f' {meta_data[-1]} : {payload_data},')
	data_element_idx += datatype_size[meta_data[1]]
	return data_element_idx

def decode_data(data_list, json_data, decoded_file, event_list, endianness, events_skipped):

	data_element_idx = 0
	event_idx = 0
	structure_list = list(json_data['STRUCTURES'].items())
	json_data_list = list(json_data['EVENTS'].items())
	json_data_list_struct = list(json_data['STRUCTURES'].items())[1]
	datatype_size = {"uint8_t" : 1, "uint16_t" : 2, "uint32_t" : 4, "uint64_t": 8}
	formatter = {"uint8_t" : 'B', "uint16_t" : 'H', "uint32_t" : 'I', "uint64_t": 'Q'}
	while(data_element_idx < len(data_list)):
		event_idx = 0
		event_decoded_flag = 0
		json_iterated_events = set()
		while(event_idx < len(json_data_list) and event_idx not in json_iterated_events):
			data_size = 2
			payload_data = data_list[data_element_idx : data_element_idx + data_size ]
			format_endianness = endianness + formatter["uint16_t"]
			event_id = struct.unpack(format_endianness, bytes(payload_data))[0]
			if(event_id >= (len(event_list) - 1)):
				break
			event_id_from_json = event_list.index(json_data_list[event_idx][0])
			if (event_id == event_id_from_json):
				event_decoded_flag = 1
				data_element_idx += 2
				data_size = 2
				payload_data = data_list[data_element_idx : data_element_idx + data_size ]
				format_endianness = endianness + formatter["uint16_t"]
				payload_length = struct.unpack(format_endianness, bytes(payload_data))[0]
				data_element_idx += 2
				cand_elem_idx = data_element_idx
				for field, data in json_data_list[event_idx][1][0].items():
					if (field == 'event_info'):
						event_name = data[-1]
					if (field == "cmn"):
						for field, data in  json_data_list_struct[1][0].items():
							if (field != 'hdr'):
								data_size = datatype_size[data[1]]
								payload_data = data_list[data_element_idx : data_element_idx + data_size ]
								format_endianness = endianness + formatter[data[1]]
								payload_data = struct.unpack(format_endianness, bytes(payload_data))[0]
								if(field == 'time_tick'):
									time_tick = f"{payload_data : #013x}"
									data_element_idx += datatype_size[data[1]]
									continue
								elif(field == "timestamp_us"):
									timestamp = timestamp_calculator(payload_data)
									data_element_idx += datatype_size[data[1]]
									continue
								elif(field == "opmode"):
									opmode = payload_data
								else:
									vdev_id = payload_data
								data_element_idx += datatype_size[data[1]]
						decoded_file.write('')
						decoded_file.write(f'\n[{timestamp}] [{time_tick}] [event : {event_name}({event_id})] [vdev_id : {vdev_id}] [opmode :  {opmode}]')
					elif (data[0] == "struct" and len(data) == 2):
						for key, value in structure_list:
							if (data[1] == key):
								for field, struct_data in value[0].items():
									if (struct_data[0] == 0):
										if(field == 'ssid'):
											data_element_idx = decode_ssid(data_list, struct_data, data_element_idx, ssid_len)
										elif(field == 'bssid' or field == 'sa' or field == 'dest_mac' or field == 'init_sta_addr' or field == 'resp_sta_addr' or field == 'cc' or field == 'sta_mac' or field == 'self_ndi_mac_addr' or field == 'peer_discovery_mac_addr' or field == 'peer_mac' or field == 'src_mac' or field == 'dst_mac'):
											data_element_idx =  decode_macaddr(data_list, struct_data, data_element_idx)
										elif (field == "scanned_freq"):
											data_element_idx = scanned_freq_decoder(data_list, struct_data, data_element_idx, num_scanned_freq)
										else:
											data_element_idx = decode_array(data_list, struct_data, data_element_idx, endianness)
									elif (struct_data[0] == 1):
										if(field == "ssid_len"):
											ssid_len = data_list[data_element_idx]
											decoded_file.write(f' {struct_data[-1]} : {ssid_len},')
											data_element_idx += datatype_size[struct_data[1]]
										elif(field == "flags"):
											data_element_idx = decode_flags(data_list, struct_data, data_element_idx)
										elif(field == "num_scanned_freq"):
											data_element_idx, num_scanned_freq = decode_num_scanned_freq(data_list, struct_data, data_element_idx, endianness)
										else:
											data_element_idx = decode_integer(data_list, struct_data, data_element_idx, endianness)
					elif (data[0] == 'struct' and len(data) == 4):
						if (data[1] == 'cstats_sta_roam_scan_ap'):
							size = cand_ap_count
						else:
							size = data[2]
						for key, value in structure_list:
							if(data[1] == key):
								struct_idx = 0
								while(struct_idx < size):
									for field,struct_data in value[0].items():
										if (struct_data[0] == 0):
											if(field == 'ssid'):
												data_element_idx = decode_ssid(data_list, struct_data, data_element_idx, ssid_len)
											elif(field == 'bssid'or field == 'sa' or field == 'dest_mac' or field == 'init_sta_addr' or field == 'resp_sta_addr' or field == 'cc' or field == 'sta_mac' or field == 'self_ndi_mac_addr' or field == 'peer_discovery_mac_addr' or field == 'peer_mac' or field == 'src_mac' or field == 'dst_mac'):
												data_element_idx =  decode_macaddr(data_list, struct_data, data_element_idx)
											else:
												data_element_idx = decode_array(data_list, struct_data, data_element_idx, enidanness)
										elif (struct_data[0] == 1):
											if(field == "ssid_len"):
												ssid_len = data_list[data_element_idx]
												decoded_file.write(f' {struct_data[-1]} : {ssid_len},')
												data_element_idx += datatype_size[struct_data[1]]
											elif(field == "flags"):
												data_element_idx = decode_flags(data_list, struct_data, data_element_idx)
											elif(field == "num_scanned_freq"):
												data_element_idx, num_scanned_freq = decode_num_scanned_freq(data_list, struct_data, data_element_idx, endianness)
											else:
												data_element_idx = decode_integer(data_list, struct_data, data_element_idx, endianness)
									struct_idx += 1
						if (data[1] == 'cstats_sta_roam_scan_ap'):
							data_element_idx = cand_elem_idx + payload_length - 1
					elif (field != "event_info"):
						if (data[0] == 0):
							if(field == 'ssid'):
								data_element_idx = decode_ssid(data_list, data, data_element_idx, ssid_len)
							elif(field == 'bssid' or field == 'sa' or field == 'dest_mac' or field == 'init_sta_addr' or field == 'resp_sta_addr' or field == 'cc' or field == 'sta_mac' or field == 'self_ndi_mac_addr' or field == 'peer_discovery_mac_addr' or field == 'peer_mac' or field == 'src_mac' or field == 'dst_mac'):
								data_element_idx =  decode_macaddr(data_list, data, data_element_idx)
							elif (field == "scanned_freq"):
								data_element_idx = scanned_freq_decoder(data_list, data, data_element_idx, num_scanned_freq)
							else:
								data_element_idx = decode_array(data_list, data, data_element_idx, endianness)
						elif (data[0] == 1):
							if(field == "ssid_len"):
								ssid_len = data_list[data_element_idx]
								decoded_file.write(f' {data[-1]} : {ssid_len},')
								data_element_idx += datatype_size[data[1]]
							elif(field == "flags"):
								data_element_idx = decode_flags(data_list, data, data_element_idx)
							elif(field == "num_scanned_freq"):
								data_element_idx, num_scanned_freq = decode_num_scanned_freq(data_list, data, data_element_idx, endianness)
							else:
								data_size = datatype_size[data[1]]
								payload_data = data_list[data_element_idx : data_element_idx + data_size ]
								format_endianness = endianness + formatter[data[1]]
								payload_data = struct.unpack(format_endianness, bytes(payload_data))[0]
								if (field == "cand_ap_count"):
									cand_ap_count = payload_data
								decoded_file.write(f' {data[-1]} : {payload_data},')
								data_element_idx += datatype_size[data[1]]
				json_iterated_events.add(event_idx)
				break
			if (json_data_list[event_idx][0] not in event_list):
				events_skipped += 1
				data_element_idx += 2
				payload_length = data_list[data_element_idx]
				data_element_idx += payload_length + 4
				break
			event_idx += 1
		if (data_element_idx >= len(data_list) or data_list[data_element_idx] > len(json_data_list) or event_decoded_flag == 0):
			break
	return events_skipped

def decode(binay_data, json_data, decoded_file, event_list):

	start_marker_ascii = [ord(element) for element in "CS_HSM"]
	end_marker_ascii = [ord(element) for element in "CS_HEM"]
	data_list , hdr_version, endianness, drop_cnt = decode_metadata(binary_data, start_marker_ascii, end_marker_ascii)
	events_skipped = 0
	if (str(json_data["VERSION"]['CHIPSET_STATS_HDR_VERSION']) ==  hdr_version):
		decoded_file.write(f"CHIPSET_STATS_HDR_VERSION : {json_data['VERSION']['CHIPSET_STATS_HDR_VERSION']}, ")
		decoded_file.write(f"DROP COUNT : {drop_cnt} Nodes (1 node = 2048 bytes)\n")
		start_mkr_idx = 0
		end_mkr_idx = 0
		start_mkr_found_flag = 0
		for i in range(len(data_list)):
			if (data_list[i : i + len(start_marker_ascii)] == start_marker_ascii and start_mkr_found_flag == 0):
				start_mkr_idx = i
				start_mkr_found_flag = 1
			if(start_mkr_found_flag == 1 and data_list[i : i + len(end_marker_ascii)] == end_marker_ascii):
				end_mkr_idx = i
				start_mkr_found_flag = 0
				data_segment = data_list[start_mkr_idx + len(start_marker_ascii) : end_mkr_idx]
				events_skipped = decode_data(data_segment, json_data, decoded_file, event_list, endianness, events_skipped)
		decoded_file.write(f'\nSkipped events : {events_skipped}')
		decoded_file.close()
	else :
		decoded_file.write(f"CHIPSET_STATS_HDR_VERSION IN JSON : {json_data['VERSION']['CHIPSET_STATS_HDR_VERSION']}\n")
		decoded_file.write(f"CHIPSET_STATS_HDR_VERSION IN DUMP : {hdr_version}\n")
		print("version mismatch")

if __name__ == "__main__":

	if (len(sys.argv) == 1 or len(sys.argv) > 4):
		print ("\nInvalid Arguments, Use 'Python parser_and_decoder_script.py -h' to get valid commands\n")
	elif (len(sys.argv) == 2 and sys.argv[1] == '-h'):
		print("\n--Header Generation--")
		print("Python parser_and_decoder_script.py -g <json_file>")
		print("\n--log Generation--")
		print("Python parser_and_decoder_script.py -d <json_file> <debug_dump>\n")
	elif (len(sys.argv) == 3 or len(sys.argv) == 4):
		if (sys.argv[1] == '-g'):
			json_file = sys.argv[2]
			json_data = read_json(json_file)
			generate_file(json_data)
		elif (sys.argv[1] == '-d'):
			json_file = sys.argv[2]
			json_data = read_json(json_file)
			event_list = get_events(json_data)
			dump_file = sys.argv[3]
			binary_data, decoded_file = read_binary(dump_file)
			decode(binary_data, json_data, decoded_file, event_list)
		else:
			print("\nInvalid Arguments, Use 'Python parser_and_decoder_script.py -h' to get valid commands\n")
	else:
		print("\nInvalid Arguments, Use 'Python parser_and_decoder_script.py -h' to get valid commands\n")

