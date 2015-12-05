#pragma once

#include <unordered_map>
#include <vector>

#include "View2.h"
#include "ViewInfo.h"

class GraphicRepresentation
{
public:
    GraphicRepresentation()
    {
        current_frame_ = &first_data_;
        new_frame_ = &second_data_;
        is_updated_ = false;
        current_frame_id_ = 1;
        pixel_movement_tick_ = SDL_GetTicks();
    }

    struct Entity
    {
        Entity()
        {
            id = 0;
            pos_x = 0;
            pos_y = 0;
            vlevel = 0;
            dir = D_DOWN;
        }

        ViewInfo view;
        size_t id;
        int pos_x;
        int pos_y;
        int vlevel;
        int dir;
    };
    void AddToNewFrame(const Entity& ent)
    {
        new_frame_->entities.push_back(ent);
    }
    void SetCameraForFrame(int pos_x, int pos_y)
    {
        new_frame_->camera_pos_x = pos_x;
        new_frame_->camera_pos_y = pos_y;
    }

    void Swap()
    {
        // TODO: mutex
        // Lock(frames_mutex_);
        std::swap(current_frame_, new_frame_);
        is_updated_ = true;
        new_frame_->entities.clear();
    }
    void Process();
private:
    void SynchronizeViews();
    void PerformPixelMovement();
    void Draw();

    int pixel_movement_tick_;

    size_t current_frame_id_;

    bool is_updated_;

    struct FrameData
    {
        std::vector<Entity> entities;
        int camera_pos_x;
        int camera_pos_y;
    };

    typedef FrameData DataType;

    DataType* current_frame_;
    DataType* new_frame_;

    DataType first_data_;
    DataType second_data_;

    struct ViewWithFrameId
    {
        View2 view;
        size_t frame_id;
    };

    std::unordered_map<size_t, ViewWithFrameId> views_;

    struct CameraData
    {
        CameraData()
        {
            pos_x = 0;
            pos_y = 0;
            pixel_shift_x_ = 0;
            pixel_shift_y_ = 0;
        }

        int pos_x;
        int pos_y;

        int pixel_shift_x_;
        int pixel_shift_y_;
    } camera_;
};

GraphicRepresentation &GetGraphicRepresentation();
void SetGraphicRepresentation(GraphicRepresentation* g_r);
