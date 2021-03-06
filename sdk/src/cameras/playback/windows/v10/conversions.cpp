// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2016 Intel Corporation. All Rights Reserved.

#include "windows/v10/conversions.h"
#include "include/linear_algebra.h"
#include "rs/utils/librealsense_conversion_utils.h"
#include "rs/utils/log_utils.h"
#include "rs/core/image_interface.h"

using namespace rs::core;

namespace rs
{
    namespace playback
    {
        namespace windows
        {
            namespace v10
            {
                namespace conversions
                {

                    uint64_t rssdk2lrs_timestamp(uint64_t time)
                    {
                        return static_cast<uint64_t>(static_cast<double>(time) * 0.1);
                    }

                    core::status convert(file_types::stream_type source, rs_stream &target)
                    {
                        target = rs_stream::RS_STREAM_COUNT;
                        switch(source)
                        {
                            case file_types::stream_type::stream_type_color: target = rs_stream::RS_STREAM_COLOR; break;
                            case file_types::stream_type::stream_type_depth: target = rs_stream::RS_STREAM_DEPTH; break;
                            case file_types::stream_type::stream_type_ir:    target = rs_stream::RS_STREAM_INFRARED; break;
                            case file_types::stream_type::stream_type_left:  target = rs_stream::RS_STREAM_INFRARED; break;
                            case file_types::stream_type::stream_type_right: target = rs_stream::RS_STREAM_INFRARED2; break;
                            case file_types::stream_type::stream_type_aux_color: return core::status_feature_unsupported;
                            default: return core::status_item_unavailable; break;
                        }
                        return core::status_no_error;
                    }

                    core::status convert(file_types::compression_type source, core::file_types::compression_type &target)
                    {
                        target = core::file_types::compression_type::compression_type_invalid_value;
                        switch(source)
                        {
                            case file_types::compression_none: target = core::file_types::compression_type::none; break;
                            case file_types::compression_h264: target = core::file_types::compression_type::h264; break;
                            case file_types::compression_lzo: target = core::file_types::compression_type::lzo; break;
                            default: return core::status_item_unavailable; break;
                        }
                        return core::status_no_error;
                    }

                    core::status convert(file_types::rotation source, rs::core::rotation &target)
                    {
                        target = rs::core::rotation::rotation_invalid_value;
                        switch(source)
                        {
                            case file_types::rotation:: rotation_0_degree: target = rs::core::rotation::rotation_0_degree; break;
                            case file_types::rotation:: rotation_90_degree: target = rs::core::rotation::rotation_90_degree; break;
                            case file_types::rotation:: rotation_180_degree: target = rs::core::rotation::rotation_180_degree; break;
                            case file_types::rotation:: rotation_270_degree: target = rs::core::rotation::rotation_270_degree; break;
                            default: return core::status_item_unavailable; break;
                        }
                        return core::status_no_error;
                    }

                    core::status convert(file_types::pixel_format source, rs_format &target)
                    {
                        target = rs_format::RS_FORMAT_COUNT;
                        switch(source)
                        {
                            case 	 file_types::pixel_format::pf_any:        target = rs_format::RS_FORMAT_ANY; break;
                            case 	 file_types::pixel_format::pf_depth:      target = rs_format::RS_FORMAT_Z16; break;
                            case     file_types::pixel_format::pf_depth_f32:  target = rs_format::RS_FORMAT_XYZ32F; break;
                            case 	 file_types::pixel_format::pf_yuy2:       target = rs_format::RS_FORMAT_YUYV; break;
                            case 	 file_types::pixel_format::pf_rgb:        target = rs_format::RS_FORMAT_RGB8; break;
                            case 	 file_types::pixel_format::pf_rgba:       target = rs_format::RS_FORMAT_RGBA8; break;
                            case 	 file_types::pixel_format::pf_bgr:        target = rs_format::RS_FORMAT_BGR8; break;
                            case 	 file_types::pixel_format::pf_bgra:       target = rs_format::RS_FORMAT_BGRA8; break;
                            case 	 file_types::pixel_format::pf_y8:         target = rs_format::RS_FORMAT_Y8; break;
                            case 	 file_types::pixel_format::pf_y16:        target = rs_format::RS_FORMAT_Y16; break;
                            case 	 file_types::pixel_format::pf_raw:        target = rs_format::RS_FORMAT_RAW10; break;
                            default:                                       return core::status_item_unavailable; break;
                        }
                        return core::status_no_error;
                    }

                    core::status convert(const file_types::coordinate_system &source, core::file_types::coordinate_system &target)
                    {
                        target = core::file_types::coordinate_system::coordinate_system_invalid_value;
                        switch(source)
                        {
                            case file_types::coordinate_system::coordinate_system_rear_default:  target = core::file_types::coordinate_system::rear_default; break;
                            case file_types::coordinate_system::coordinate_system_rear_opencv:   target = core::file_types::coordinate_system::rear_opencv; break;
                            case file_types::coordinate_system::coordinate_system_front_default: target = core::file_types::coordinate_system::front_default; break;
                            default:                                                target = (core::file_types::coordinate_system)source; break;
                        }
                        return core::status_no_error;
                    }

                    core::status convert(const file_types::disk_format::header &source, core::file_types::file_header &target)
                    {
                        memset(&target, 0, sizeof(target));
                        core::file_types::coordinate_system cs;
                        if(convert(source.coordinate_system, cs) != core::status_no_error)
                            return core::status_item_unavailable;
                        target.id = source.id;
                        target.version = source.version;
                        target.coordinate_system = cs;
                        target.first_frame_offset = source.first_frame_offset;
                        target.nstreams = source.nstreams;
                        return core::status_no_error;
                    }

                    core::status convert(const file_types::disk_format::stream_info &source, core::file_types::stream_info &target)
                    {
                        memset(&target, 0, sizeof(target));
                        rs_stream stream;
                        core::file_types::compression_type ctype;
                        auto stream_sts = convert(source.stype, stream);
                        if(stream_sts != core::status_no_error)
                            return stream_sts;
                        auto compression_sts = convert(source.ctype, ctype);
                        if(compression_sts != core::status_no_error)
                            return compression_sts;
                        target.stream = stream;
                        target.nframes = source.nframes;
                        target.ctype = ctype;
                        return core::status_no_error;
                    }

                    core::status convert(const file_types::disk_format::device_info_disk &source, std::map<rs_camera_info, std::string>& target)
                    {
                        rs::core::rotation rotaion;
                        if(convert(source.rotation, rotaion) != core::status_no_error)
                            return core::status_item_unavailable;

                        std::stringstream name;
                        auto nameSize = sizeof(source.name) / sizeof(source.name[0]);
                        for(size_t i = 0; i < nameSize && source.name[i] != 0; i++)
                        {
                            name << static_cast<char>(source.name[i]);
                        }
                        target[rs_camera_info::RS_CAMERA_INFO_DEVICE_NAME] = name.str();

                        std::stringstream serial;
                        auto serialSize = sizeof(source.serial) / sizeof(source.serial[0]);
                        for(size_t i = 0; i < serialSize && source.serial[i] != 0; i++)
                        {
                            serial << static_cast<char>(source.serial[i]);
                        }
                        target[rs_camera_info::RS_CAMERA_INFO_DEVICE_SERIAL_NUMBER] = serial.str();

                        std::stringstream ss;
                        ss << source.firmware[0] << "." << source.firmware[1] << "." <<
                           source.firmware[2] << "." << source.firmware[3];
                        target[rs_camera_info::RS_CAMERA_INFO_CAMERA_FIRMWARE_VERSION] = ss.str();
                        return core::status_no_error;
                    }

                    core::status convert(const file_types::image_info &source, core::file_types::frame_info &target)
                    {
                        memset(&target, 0, sizeof(target));
                        rs_format format;
                        if(convert(source.format, format) != core::status_no_error)
                            return core::status_item_unavailable;

                        const int pixel_size = get_pixel_size(utils::convert_pixel_format(static_cast<rs::format>(format)));
                        target.width = source.width;
                        target.height = source.height;
                        target.stride = (source.width == 628 ? 640 : source.width) * pixel_size;
                        target.bpp = pixel_size;
                        target.format = format;
                        return core::status_no_error;
                    }

                    core::status convert(const file_types::disk_format::stream_profile_disk &source, core::file_types::stream_profile &target)
                    {
                        memset(&target, 0, sizeof(target));
                        core::file_types::frame_info frame_info;
                        if(convert(source.image_info, frame_info) != core::status_no_error)
                            return core::status_item_unavailable;
                        if(source.frame_rate[0] != source.frame_rate[1])
                        {
                            LOG_ERROR("min != max fps is not supported, setting to min");
                        }
                        target.frame_rate = static_cast<int32_t>(source.frame_rate[0]);
                        target.info = frame_info;
                        target.intrinsics.width = frame_info.width;
                        target.intrinsics.height = frame_info.height;

                        return core::status_no_error;
                        //rv.options = profile.options;
                    }

                    core::status convert(const file_types::disk_format::stream_profile_set_disk &source, std::map<rs_stream, core::file_types::stream_info> &target)
                    {
                        core::file_types::stream_profile sp;
                        if(source.color.image_info.format)
                        {
                            if(convert(source.color, sp) != core::status_no_error) return core::status_item_unavailable;
                            target[rs_stream::RS_STREAM_COLOR].profile = sp;
                        }
                        if(source.depth.image_info.format)
                        {
                            if(convert(source.depth, sp) != core::status_no_error) return core::status_item_unavailable;
                            target[rs_stream::RS_STREAM_DEPTH].profile = sp;
                        }
                        if(source.ir.image_info.format)
                        {
                            if(convert(source.ir, sp) != core::status_no_error) return core::status_item_unavailable;
                            target[rs_stream::RS_STREAM_INFRARED].profile = sp;
                        }
                        if(source.left.image_info.format)
                        {
                            if(convert(source.left, sp) != core::status_no_error) return core::status_item_unavailable;
                            target[rs_stream::RS_STREAM_INFRARED].profile = sp;
                        }
                        if(source.right.image_info.format)
                        {
                            if(convert(source.right, sp) != core::status_no_error) return core::status_item_unavailable;
                            target[rs_stream::RS_STREAM_INFRARED2].profile = sp;
                        }
                        return core::status_no_error;
                    }

                    core::status convert(const file_types::disk_format::frame_metadata &source, core::file_types::frame_info &target)
                    {
                        memset(&target, 0, sizeof(target));
                        rs_stream stream;
                        if(convert(source.stream_type, stream) != core::status_no_error)
                            return core::status_item_unavailable;
                        target.stream = stream;
                        target.time_stamp = static_cast<double>(rssdk2lrs_timestamp(source.time_stamp));
                        target.number = source.frame_number;
                        return core::status_no_error;
                    }

                    rs_intrinsics get_intrinsics(file_types::stream_type stream, ds_projection::ProjectionData* projection)
                    {
                        rs::intrinsics rv;
                        switch(stream)
                        {
                            case file_types::stream_type::stream_type_color:
                            {
                                rv.fx = projection->thirdCameraParams.isRectified ?
                                        projection->thirdCameraParams.calibIntrinsicsRectified.rfx : projection->thirdCameraParams.calibIntrinsicsNonRectified.fx;
                                rv.fy = projection->thirdCameraParams.isRectified ?
                                        projection->thirdCameraParams.calibIntrinsicsRectified.rfy : projection->thirdCameraParams.calibIntrinsicsNonRectified.fy;
                                rv.ppx = projection->thirdCameraParams.isRectified ?
                                         projection->thirdCameraParams.calibIntrinsicsRectified.rpx : projection->thirdCameraParams.calibIntrinsicsNonRectified.px;
                                rv.ppy = projection->thirdCameraParams.isRectified ?
                                         projection->thirdCameraParams.calibIntrinsicsRectified.rpy : projection->thirdCameraParams.calibIntrinsicsNonRectified.py;
                                rv.width = projection->thirdCameraParams.width;
                                rv.height = projection->thirdCameraParams.height;
                                break;
                            }
                            case file_types::stream_type::stream_type_depth:
                            {
                                rv.fx = projection->zRectified ? projection->zIntrinRect.rfx : projection->zIntrinNonRect.fx;
                                rv.fy = projection->zRectified ? projection->zIntrinRect.rfy : projection->zIntrinNonRect.fy;
                                rv.ppx = projection->zRectified ? projection->zIntrinRect.rpx : projection->zIntrinNonRect.px;
                                rv.ppy = projection->zRectified ? projection->zIntrinRect.rpy : projection->zIntrinNonRect.py;
                                rv.width = projection->dWidth;
                                rv.height = projection->dHeight;
                                break;
                            }
                            case file_types::stream_type::stream_type_left:
                            {
                                rv.fx = projection->zRectified ? projection->lrIntrinRect.rfx : projection->lIntrinNonRect.fx;
                                rv.fy = projection->zRectified ? projection->lrIntrinRect.rfy : projection->lIntrinNonRect.fy;
                                rv.ppx = projection->zRectified ? projection->lrIntrinRect.rpx : projection->lIntrinNonRect.px;
                                rv.ppy = projection->zRectified ? projection->lrIntrinRect.rpy : projection->lIntrinNonRect.py;
                                rv.width = projection->dWidth;
                                rv.height = projection->dHeight;
                                break;
                            }
                            case file_types::stream_type::stream_type_right:
                            {
                                rv.fx = projection->zRectified ? projection->lrIntrinRect.rfx : projection->rIntrinNonRect.fx;
                                rv.fy = projection->zRectified ? projection->lrIntrinRect.rfy : projection->rIntrinNonRect.fy;
                                rv.ppx = projection->zRectified ? projection->lrIntrinRect.rpx : projection->rIntrinNonRect.px;
                                rv.ppy = projection->zRectified ? projection->lrIntrinRect.rpy : projection->rIntrinNonRect.py;
                                rv.width = projection->dWidth;
                                rv.height = projection->dHeight;
                                break;
                            }
                            default: break;
                        }
                        return rv;
                    }

                    rs_extrinsics get_extrinsics(file_types::stream_type stream, ds_projection::ProjectionData *projection)
                    {
                        rs_extrinsics rv = {};
                        switch(stream)
                        {
                            case file_types::stream_type::stream_type_color:
                            {
                                rs::utils::float3 trans = {};
                                for(int i = 0; i < 3; ++i)
                                {
                                    trans[i] = static_cast<float>(projection->thirdCameraParams.isRectified ?
                                               projection->thirdCameraParams.zToRectColorTranslation[i] * 0.001 :
                                               projection->thirdCameraParams.zToNonRectColorTranslation[i] * 0.001);
                                }
                                rs::utils::float3x3 rot = {};
                                memcpy(&rot, projection->calibParams.Rthird[0], sizeof(float) * 9);
                                const rs::utils::pose mat = {rot, trans};
                                auto inv = inverse(mat);
                                rv = {inv.orientation.x.x, inv.orientation.x.y, inv.orientation.x.z,
                                      inv.orientation.y.x, inv.orientation.y.y, inv.orientation.y.z,
                                      inv.orientation.z.x, inv.orientation.z.y, inv.orientation.z.z,
                                      inv.position.x, inv.position.y, inv.position.z
                                     };
                                return rv;
                                break;
                            }
                            default: break;//TODO - add support in other streams
                        }
                        return rv;
                    }
                }
            }
        }
    }
}
