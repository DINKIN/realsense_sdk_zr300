// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2016 Intel Corporation. All Rights Reserved.

#pragma once
#include <memory>
#include <map>
#include <fstream>
#include <algorithm>
#include <librealsense/rs.hpp>
#include "rs/core/image_interface.h"
#include "rs/utils/librealsense_conversion_utils.h"
#include "rs/utils/smart_ptr_helpers.h"
#include "rs/utils/self_releasing_array_data_releaser.h"

namespace test_utils
{
    static std::shared_ptr<rs::core::image_interface> create_image(rs::device * device, rs::stream stream)
    {
        if(device->get_frame_data(stream) == nullptr)
            return nullptr;
        auto sdk_stream = rs::utils::convert_stream_type(stream);
        auto sdk_format = rs::utils::convert_pixel_format(device->get_stream_format(stream));
        const int pitch = device->get_stream_width(stream) * rs::core::get_pixel_size(sdk_format);
        rs::core::image_info info = {device->get_stream_width(stream),
                                     device->get_stream_height(stream),
                                     sdk_format,
                                     pitch
                                    };
        auto size = pitch * device->get_stream_height(stream);
        uint8_t * dst_data = new uint8_t[size];
        memcpy(dst_data, device->get_frame_data(stream), size);
        auto data_releaser = new rs::utils::self_releasing_array_data_releaser(dst_data);
        return rs::utils::get_shared_ptr_with_releaser(rs::core::image_interface::create_instance_from_raw_data(&info,
                                                                                      {dst_data, data_releaser},
                                                                                      sdk_stream,
                                                                                      rs::core::image_interface::flag::any,
                                                                                      device->get_frame_timestamp(stream),
                                                                                      device->get_frame_number(stream)));
    }
    
    static std::map<std::string, std::string> parse_configuration_file(std::string conf_file_path)
    {
        std::map<std::string, std::string> pairs;
        
        std::ifstream conf_file(conf_file_path);
        if(conf_file.good())
        {
            std::string line;
            while (std::getline(conf_file, line))
            {
                //remove whitespaces
                auto removed = std::remove_if(line.begin(), line.end(), ::isspace);
                line.erase(removed, line.end());
        
                std::string key;
                std::istringstream iss_line(line);
                if (std::getline(iss_line, key, '='))
                {
                    if (key[0] == '#') //comment line
                    {
                        continue;
                    }
                    std::string value;
                    if (std::getline(iss_line, value))
                    {
                        pairs[key] = value;
                    }
                }
            }
        }
        return pairs;
    }

}


