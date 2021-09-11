#include "BobClass.h"
#include "pch.h"

/*
extern LPDIRECTDRAW7        lpdd;  // dd object
extern LPDIRECTDRAWSURFACE7 lpddsprimary;  // dd primary surface
extern LPDIRECTDRAWSURFACE7 lpddsback;  // dd back surface
extern LPDIRECTDRAWCLIPPER  lpddclipper;   // dd clipper for back surface
extern LPDIRECTDRAWCLIPPER  lpddclipperwin; // dd clipper for window

extern DDSURFACEDESC2       ddsd;                 // a direct draw surface description struct
extern DDBLTFX              ddbltfx;              // used to fill
extern DDSCAPS2             ddscaps;              // a direct draw surface capabilities struct
extern HRESULT              ddrval;               // result back from dd calls
extern UINT* primary_buffer; // primary video buffer
extern UINT* back_buffer; // secondary back buffer

extern HWND main_window_handle; // save the window handle
extern HINSTANCE main_instance; // save the instance

extern int screen_width,            // width of screen
screen_height;           // height of screen

extern int min_clip_x,                             // clipping rectangle
max_clip_x,
min_clip_y,
max_clip_y;

extern int window_client_x0;   // used to track the starting (x,y) client area for
extern int window_client_y0;   // for windowed mode directdraw operations
*/


CREATE_BOB_OBJECTS()

// DEFINES ////////////////////////////////////////////////
UINT RGBBIT(UCHAR a, UCHAR r, UCHAR g, UCHAR b) { return b + (g << 8) + (r << 16) + (a << 24); }

int DDraw_Init(int width, int height)
{
    if (FAILED(DirectDrawCreateEx(NULL, (void**)&lpdd, IID_IDirectDraw7, NULL)))
        return(0);

    if (FAILED(lpdd->SetCooperativeLevel(main_window_handle, DDSCL_NORMAL)))
        return(0);

    // set globals
    screen_height = height;
    screen_width = width;

    // Create the primary surface
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    // windowed mode
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    // set the backbuffer count to 0 for windowed mode
    ddsd.dwBackBufferCount = 0;

    // create the primary surface
    lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL);

    // get the pixel format of the primary surface
    DDPIXELFORMAT ddpf; // used to get pixel format

    // initialize structure
    DDRAW_INIT_STRUCT(ddpf);

    // query the format from primary surface
    lpddsprimary->GetPixelFormat(&ddpf);

    // must be windowed, so create a double buffer that will be blitted
    // rather than flipped as in full screen mode
    lpddsback = DDraw_Create_Surface(width, height); // int mem_flags, USHORT color_key_flag);
    // clear out both primary and secondary surfaces
    DDraw_Fill_Surface(lpddsback, 0);

    // set software algorithmic clipping region
    min_clip_x = 0;
    max_clip_x = screen_width - 1;
    min_clip_y = 0;
    max_clip_y = screen_height - 1;

    // setup backbuffer clipper always
    RECT screen_rect = { 0,0,screen_width,screen_height };
    lpddclipper = DDraw_Attach_Clipper(lpddsback, 1, &screen_rect);

    // set windowed clipper
    if (FAILED(lpdd->CreateClipper(0, &lpddclipperwin, NULL)))
        return(0);

    if (FAILED(lpddclipperwin->SetHWnd(0, main_window_handle)))
        return(0);

    if (FAILED(lpddsprimary->SetClipper(lpddclipperwin)))
        return(0);

    // return success
    return(1);
} // end DDraw_Init

///////////////////////////////////////////////////////////

LPDIRECTDRAWSURFACE7 DDraw_Create_Surface(int width,
    int height,
    int mem_flags,
    UINT color_key_low,
    UINT color_key_high)
{
    // this function creates an offscreen plain surface

    DDSURFACEDESC2 ddsd;         // working description
    LPDIRECTDRAWSURFACE7 lpdds;  // temporary surface

    // set to access caps, width, and height
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CKSRCBLT;

    // set dimensions of the new bitmap surface
    ddsd.dwWidth = width;
    ddsd.dwHeight = height;

    // set surface to offscreen plain
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | mem_flags;

    // create the surface
    if (FAILED(lpdd->CreateSurface(&ddsd, &lpdds, NULL)))
        return(NULL);

    DDraw_SetColorKey(lpdds, color_key_low, color_key_high);

    // return surface
    return(lpdds);
} // end DDraw_Create_Surface

///////////////////////////////////////////////////////////

int DDraw_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds, USHORT color, RECT* client)
{
    DDBLTFX ddbltfx; // this contains the DDBLTFX structure

    // clear out the structure and set the size field 
    DDRAW_INIT_STRUCT(ddbltfx);

    // set the dwfillcolor field to the desired color
    ddbltfx.dwFillColor = color;

    // ready to blt to surface
    lpdds->Blt(client,     // ptr to dest rectangle
        NULL,       // ptr to source surface, NA            
        NULL,       // ptr to source rectangle, NA
        DDBLT_COLORFILL | DDBLT_WAIT,   // fill and wait                   
        &ddbltfx);  // ptr to DDBLTFX structure

// return success
    return(1);
} // end DDraw_Fill_Surface

///////////////////////////////////////////////////////////

LPDIRECTDRAWCLIPPER DDraw_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds,
    int num_rects,
    LPRECT clip_list)

{
    // this function creates a clipper from the sent clip list and attaches
    // it to the sent surface

    int index;                         // looping var
    LPDIRECTDRAWCLIPPER lpddclipper;   // pointer to the newly created dd clipper
    LPRGNDATA region_data;             // pointer to the region data that contains
                                       // the header and clip list

    // first create the direct draw clipper
    if (FAILED(lpdd->CreateClipper(0, &lpddclipper, NULL)))
        return(NULL);

    // now create the clip list from the sent data

    // first allocate memory for region data
    region_data = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER) + num_rects * sizeof(RECT));

    // now copy the rects into region data
    memcpy(region_data->Buffer, clip_list, sizeof(RECT) * num_rects);

    // set up fields of header
    region_data->rdh.dwSize = sizeof(RGNDATAHEADER);
    region_data->rdh.iType = RDH_RECTANGLES;
    region_data->rdh.nCount = num_rects;
    region_data->rdh.nRgnSize = num_rects * sizeof(RECT);

    region_data->rdh.rcBound.left = 64000;
    region_data->rdh.rcBound.top = 64000;
    region_data->rdh.rcBound.right = -64000;
    region_data->rdh.rcBound.bottom = -64000;

    // find bounds of all clipping regions
    for (index = 0; index < num_rects; index++)
    {
        // test if the next rectangle unioned with the current bound is larger
        if (clip_list[index].left < region_data->rdh.rcBound.left)
            region_data->rdh.rcBound.left = clip_list[index].left;

        if (clip_list[index].right > region_data->rdh.rcBound.right)
            region_data->rdh.rcBound.right = clip_list[index].right;

        if (clip_list[index].top < region_data->rdh.rcBound.top)
            region_data->rdh.rcBound.top = clip_list[index].top;

        if (clip_list[index].bottom > region_data->rdh.rcBound.bottom)
            region_data->rdh.rcBound.bottom = clip_list[index].bottom;

    } // end for index

// now we have computed the bounding rectangle region and set up the data
// now let's set the clipping list

    if (FAILED(lpddclipper->SetClipList(region_data, 0)))
    {
        // release memory and return error
        free(region_data);
        return(NULL);
    } // end if

 // now attach the clipper to the surface
    if (FAILED(lpdds->SetClipper(lpddclipper)))
    {
        // release memory and return error
        free(region_data);
        return(NULL);
    } // end if

 // all is well, so release memory and send back the pointer to the new clipper
    free(region_data);
    return(lpddclipper);

} // end DDraw_Attach_Clipper

///////////////////////////////////////////////////////////

int DDraw_Shutdown()
{
    // this function release all the resources directdraw
    // allocated, mainly to com objects

    // release the clippers first
    if (lpddclipper)
        lpddclipper->Release();

    if (lpddclipperwin)
        lpddclipperwin->Release();

    // release the secondary surface
    if (lpddsback)
        lpddsback->Release();

    // release the primary surface
    if (lpddsprimary)
        lpddsprimary->Release();

    // finally, the main dd object
    if (lpdd)
        lpdd->Release();

    // return success
    return(1);
} // end DDraw_Shutdown

///////////////////////////////////////////////////////////


void DDraw_SetColorKey(LPDIRECTDRAWSURFACE7 lpdds, UINT color_key_low, UINT color_key_high)
{
    DDCOLORKEY color_key; // used to set color key
    color_key.dwColorSpaceLowValue = color_key_low;
    color_key.dwColorSpaceHighValue = color_key_high;

    // now set the color key for source blitting
    lpdds->SetColorKey(DDCKEY_SRCBLT, &color_key);
}


int BITMAP_FILE::Load_File(const char* filename)
{
    FILE* fp;
    if ((fp = fopen(filename, "rb")) == NULL) return 0;

    //fseek(fp, 0, SEEK_SET);

    fread(&bitmapfileheader, 14, 1, fp);
    fread(&bitmapinfoheader, sizeof(BITMAPINFOHEADER), 1, fp);

    int Height = bitmapinfoheader.biHeight;
    int Width = bitmapinfoheader.biWidth;

    buffer = (UINT*)malloc(Height * Width * sizeof(UINT));
    if (buffer == nullptr) return 0;
    fread(buffer, sizeof(UINT), Height * Width, fp);

    fclose(fp);

    Flip_Bitmap(buffer, Width, Height);
    // return success
    return(1);
} // end Load_Bitmap_File

int BITMAP_FILE::Unload_File()
{
    // this function releases all memory associated with "bitmap"
    if (buffer)
    {
        // release memory
        free(buffer);

        // reset pointer
        buffer = NULL;

    } // end if

 // return success
    return(1);

} // end Unload_Bitmap_File

///////////////////////////////////////////////////////////

int BITMAP_FILE::Flip_Bitmap(UINT* image, int width, int height)
{
    // this function is used to flip bottom-up .BMP images
    UINT* buffer; // used to perform the image processing
    buffer = (UINT*)malloc(width * height * sizeof(UINT)); // allocate the temporary buffer
    memcpy(buffer, image, width * height * sizeof(UINT)); // copy image to work area

    // flip vertically
    for (int index = 0; index < height; index++)
        memcpy(image + ((height - 1) - index) * width,
            buffer + index * width, width * sizeof(UINT));

    // release the memory
    free(buffer);

    // return success
    return(1);

} // end Flip_Bitmap

///////////////////////////////////////////////////////////

int BOB::Create(           // the bob to create
    int _x, int _y,          // initial posiiton
    int _width, int _height, // size of bob
    int _num_frames,        // number of frames
    int _attr,              // attrs
    int mem_flags,         // memory flags in DD format
    UINT _color_key_low, // default color key low
    UINT _color_key_high // default color key high
)

{
    // Create the BOB object, note that all BOBs 
    // are created as offscreen surfaces in VRAM as the
    // default, if you want to use system memory then
    // set flags equal to:
    // DDSCAPS_SYSTEMMEMORY 
    // for video memory you can create either local VRAM surfaces or AGP 
    // surfaces via the second set of constants shown below in the regular expression
    // DDSCAPS_VIDEOMEMORY | (DDSCAPS_NONLOCALVIDMEM | DDSCAPS_LOCALVIDMEM ) 


    DDSURFACEDESC2 ddsd; // used to create surface
    int index;           // looping var

    // set state and attributes of BOB
    state = BOB_STATE_ALIVE;
    attr = _attr;
    anim_state = 0;
    counter_1 = 0;
    counter_2 = 0;
    max_count_1 = 0;
    max_count_2 = 0;

    curr_frame = 0;
    num_frames = _num_frames;
    curr_animation = 0;
    anim_counter = 0;
    anim_index = 0;
    anim_count_max = 0;
    x = _x;
    y = _y;
    xv = 0;
    yv = 0;

    // set dimensions of the new bitmap surface
    width = _width;
    height = _height;

    // set all images to null
    for (index = 0; index < MAX_BOB_FRAMES; index++)
        images[index] = NULL;

    // set all animations to null
    for (index = 0; index < MAX_BOB_ANIMATIONS; index++)
        animations[index] = NULL;

    // now create each surface
    for (index = 0; index < num_frames; index++)
    {
        // set to access caps, width, and height
        memset(&ddsd, 0, sizeof(ddsd));
        ddsd.dwSize = sizeof(ddsd);
        ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

        ddsd.dwWidth = width;
        ddsd.dwHeight = height;

        // set surface to offscreen plain
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | mem_flags;

        // create the surfaces, return failure if problem
        if (FAILED(lpdd->CreateSurface(&ddsd, images + index, NULL)))
            return(0);

        DDraw_SetColorKey(images[index], _color_key_low, _color_key_high);
    } // end for index

// return success
    return(1);

} // end Create_BOB

///////////////////////////////////////////////////////////

int Clone(BOB_PTR source, BOB_PTR dest)
{
    // this function clones a BOB and updates the attr var to reflect that
    // the BOB is a clone and not real, this is used later in the destroy
    // function so a clone doesn't destroy the memory of a real bob

    if ((source && dest) && (source != dest))
    {
        // copy the bob data
        memcpy(dest, source, sizeof(BOB));

        // set the clone attribute
        dest->attr |= BOB_ATTR_CLONE;

    } // end if

    else
        return(0);

    // return success
    return(1);

} // end Clone_BOB

///////////////////////////////////////////////////////////

int BOB::Destroy()
{
    // destroy the BOB, tests if this is a real bob or a clone
    // if real then release all the memory, otherwise, just resets
    // the pointers to null

    int index; // looping var

    // test if this is a clone
    if (attr && BOB_ATTR_CLONE)
    {
        // null link all surfaces
        for (index = 0; index < MAX_BOB_FRAMES; index++)
            if (images[index])
                images[index] = NULL;

        // release memory for animation sequences 
        for (index = 0; index < MAX_BOB_ANIMATIONS; index++)
            if (animations[index])
                animations[index] = NULL;

    } // end if
    else
    {
        // destroy each bitmap surface
        for (index = 0; index < MAX_BOB_FRAMES; index++)
            if (images[index])
                (images[index])->Release();

        // release memory for animation sequences 
        for (index = 0; index < MAX_BOB_ANIMATIONS; index++)
            if (animations[index])
                free(animations[index]);

    } // end else not clone

// return success
    return(1);
} // end Destroy_BOB

///////////////////////////////////////////////////////////

int BOB::Draw(               // bob to draw
    LPDIRECTDRAWSURFACE7 dest) // surface to draw the bob on
{
    // draw a bob at the x,y defined in the BOB
    // on the destination surface defined in dest

    RECT dest_rect,   // the destination rectangle
        source_rect; // the source rectangle                             

    // is bob visible
    if (!(attr & BOB_ATTR_VISIBLE))
        return(1);

    // fill in the destination rect
    dest_rect.left = x;
    dest_rect.top = y;
    dest_rect.right = x + width;
    dest_rect.bottom = y + height;

    // fill in the source rect
    source_rect.left = 0;
    source_rect.top = 0;
    source_rect.right = width;
    source_rect.bottom = height;

    // blt to destination surface
    if (FAILED(dest->Blt(&dest_rect, images[curr_frame],
        &source_rect, (DDBLT_WAIT | DDBLT_KEYSRC),
        NULL)))
        return(0);

    // return success
    return(1);
} // end Draw_BOB

///////////////////////////////////////////////////////////

int BOB::Draw_Scaled(int swidth, int sheight,  // bob and new dimensions
    LPDIRECTDRAWSURFACE7 dest) // surface to draw the bob on)
{
    // this function draws a scaled bob to the size swidth, sheight

    RECT dest_rect,   // the destination rectangle
        source_rect; // the source rectangle                             

    // is bob visible
    if (!(attr & BOB_ATTR_VISIBLE))
        return(1);

    // fill in the destination rect
    dest_rect.left = x;
    dest_rect.top = y;
    dest_rect.right = x + swidth;
    dest_rect.bottom = y + sheight;

    // fill in the source rect
    source_rect.left = 0;
    source_rect.top = 0;
    source_rect.right = width;
    source_rect.bottom = height;

    // blt to destination surface
    if (FAILED(dest->Blt(&dest_rect, images[curr_frame],
        &source_rect, (DDBLT_WAIT | DDBLT_KEYSRC),
        NULL)))
        return(0);

    // return success
    return(1);
} // end Draw_Scaled_BOB

///////////////////////////////////////////////////////////

int BOB::Load_Frame( // bob to load with data
    BITMAP_FILE_PTR bitmap, // bitmap to scan image data from
    int frame,       // frame to load
    int cx, int cy,   // cell or absolute pos. to scan image from
    int mode)        // if 0 then cx,cy is cell position, else 
                     // cx,cy are absolute coords
{
    // this function extracts a bitmap out of a bitmap file

    DDSURFACEDESC2 ddsd;  //  direct draw surface description 

    UINT* source_ptr,   // working pointers
        * dest_ptr;

    // test the mode of extraction, cell based or absolute
    if (mode == BITMAP_EXTRACT_MODE_CELL)
    {
        // re-compute x,y
        cx = cx * (width + 1) + 1;
        cy = cy * (height + 1) + 1;
    } // end if

    // extract bitmap data
    source_ptr = bitmap->buffer + cy * bitmap->bitmapinfoheader.biWidth + cx;
    // get the addr to destination surface memory

    // set size of the structure
    ddsd.dwSize = sizeof(ddsd);

    // lock the display surface
    (images[frame])->Lock(NULL,
        &ddsd,
        DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
        NULL);

    // assign a pointer to the memory surface for manipulation
    dest_ptr = (UINT*)ddsd.lpSurface;

    // iterate thru each scanline and copy bitmap
    int lpitch = (int)(ddsd.lPitch >> 2);
    for (int index_y = 0; index_y < height; index_y++)
    {
        memcpy(dest_ptr, source_ptr, width * sizeof(UINT)); // copy next line of data to destination

        // advance pointers
        dest_ptr += lpitch;
        source_ptr += bitmap->bitmapinfoheader.biWidth;
    } // end for index_y

    (images[frame])->Unlock(NULL); // unlock the surface 

    attr |= BOB_ATTR_LOADED; // set state to loaded

    // return success
    return(1);
} // end Load_Frame_BOB


///////////////////////////////////////////////////////////

int BOB::Animate()
{
    // this function animates a bob, basically it takes a look at
    // the attributes of the bob and determines if the bob is 
    // a single frame, multiframe, or multi animation, updates
    // the counters and frames appropriately

    // test the level of animation
    if (attr & BOB_ATTR_SINGLE_FRAME)
    {
        curr_frame = 0; // current frame always = 0
        return(1);
    } // end if
    else if (attr & BOB_ATTR_MULTI_FRAME)
    {
        if (++anim_counter >= anim_count_max) // update the counter and test if its time to increment frame
        {
            anim_counter = 0; // reset counter
            if (++curr_frame >= num_frames) // move to next frame
                curr_frame = 0;
        }
    }
    else if (attr & BOB_ATTR_MULTI_ANIM)
    {
        if (++anim_counter >= anim_count_max) // first test if its time to animate
        {
            anim_counter = 0; // reset counter
            anim_index++; // increment the animation frame index
            curr_frame = animations[curr_animation][anim_index]; // extract the next frame from animation list 
            if (curr_frame == -1) // is this and end sequence flag -1
            {
                if (attr & BOB_ATTR_ANIM_ONE_SHOT) // test if this is a single shot animation
                {
                    anim_state = BOB_STATE_ANIM_DONE; // set animation state message to done
                    anim_index--; // reset frame back one
                }
                else
                {
                    if (next_animation[curr_animation] != -1) //this is a mere-once animation 
                        curr_animation = next_animation[curr_animation];
                    anim_index = 0; // reset animation index
                }
            }
            curr_frame = animations[curr_animation][anim_index]; // extract first animation frame
        }
    }
    return(1); // return success
} // end Amimate_BOB

///////////////////////////////////////////////////////////

int BOB::Move()
{
    // this function moves the bob based on its current velocity
    // also, the function test for various motion attributes of the'
    // bob and takes the appropriate actions

    // translate the bob
    x += xv;
    y += yv;

    // test for wrap around
    if (attr & BOB_ATTR_WRAPAROUND)
    {
        // test x extents first
        if (x > max_clip_x)
            x = min_clip_x - width;
        else
            if (x < min_clip_x - width)
                x = max_clip_x;

        // now y extents
        if (x > max_clip_x)
            x = min_clip_x - width;
        else
            if (x < min_clip_x - width)
                x = max_clip_x;

    } // end if
    else
        // test for bounce
        if (attr & BOB_ATTR_BOUNCE)
        {
            // test x extents first
            if ((x > max_clip_x - width) || (x < min_clip_x))
                xv = -xv;

            // now y extents 
            if ((y > max_clip_y - height) || (y < min_clip_y))
                yv = -yv;

        } // end if

     // return success
    return(1);
} // end Move_BOB

///////////////////////////////////////////////////////////

int BOB::Load_Animation(int _anim_index, int num_frames, int* sequence, int _next_animation)
{
    // this function load an animation sequence for a bob
    // the sequence consists of frame indices, the function
    // will append a -1 to the end of the list so the display
    // software knows when to restart the animation sequence
    // indicate the next animation you want to frame or -1 for circle animation

    // allocate memory for bob animation
    if (!(animations[_anim_index] = (int*)malloc((num_frames + 1) * sizeof(int))))
        return(0);

    // load data into 
    int index;
    for (index = 0; index < num_frames; index++)
        animations[_anim_index][index] = sequence[index];

    // set the end of the list to a -1
    animations[_anim_index][index] = -1;

    // set the next_animation
    next_animation[_anim_index] = _next_animation;

    // return success
    return(1);
} // end Load_Animation_BOB

///////////////////////////////////////////////////////////

int Collision(BOB_PTR bob1, BOB_PTR bob2)
{
    if (!bob1 || !bob2) return(0); // are these a valid bobs

    // get the radi of each rect
    int width1 = (bob1->width >> 1) - (bob1->width >> 3);
    int height1 = (bob1->height >> 1) - (bob1->height >> 3);

    int width2 = (bob2->width >> 1) - (bob2->width >> 3);
    int height2 = (bob2->height >> 1) - (bob2->height >> 3);

    // compute center of each rect
    int cx1 = bob1->x + width1;
    int cy1 = bob1->y + height1;

    int cx2 = bob2->x + width2;
    int cy2 = bob2->y + height2;

    // compute deltas
    int dx = abs(cx2 - cx1);
    int dy = abs(cy2 - cy1);

    // test if rects overlap
    if (dx < (width1 + width2) && dy < (height1 + height2))
        return(1);
    else
        // else no collision
        return(0);

} // end Collision_BOBS

int DDraw_Flip(void)
{
    // this function flip the primary surface with the secondary surface

    // test if either of the buffers are locked
    if (primary_buffer || back_buffer)
        return(0);

    RECT    dest_rect;    // used to compute destination rectangle

    // get the window's rectangle in screen coordinates
    GetWindowRect(main_window_handle, &dest_rect);

    // compute the destination rectangle
    dest_rect.left += window_client_x0;
    dest_rect.top += window_client_y0;

    dest_rect.right = dest_rect.left + screen_width;
    dest_rect.bottom = dest_rect.top + screen_height;
    // clip the screen coords 
        // blit the entire back surface to the primary
    if (FAILED(lpddsprimary->Blt(&dest_rect, lpddsback, NULL, DDBLT_WAIT, NULL)))   return(0);
    // return success
    return(1);

} // end DDraw_Flip
void DDraw_Draw_Bitmap(BITMAP_FILE_PTR bitmap, LPDIRECTDRAWSURFACE7 lpdds, POINT coor)
{
    DDSURFACEDESC2 ddsd;  //  direct draw surface description 

    UINT* source_ptr,   // working pointers
        * dest_ptr;
    // extract bitmap data
    source_ptr = bitmap->buffer;
    // get the addr to destination surface memory

    // set size of the structure
    ddsd.dwSize = sizeof(ddsd);
    // lock the display surface
    if (FAILED((lpdds)->Lock(NULL,
        &ddsd,
        DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
        NULL))) return;

    // assign a pointer to the memory surface for manipulation
    dest_ptr = (UINT*)ddsd.lpSurface;
    // iterate thru each scanline and copy bitmap
    int lpitch = (int)(ddsd.lPitch >> 2);
    dest_ptr += coor.x;
    dest_ptr += lpitch * coor.y;
    for (int index_y = 0; index_y < bitmap->bitmapinfoheader.biHeight; index_y++)
    {
        memcpy(dest_ptr, source_ptr, bitmap->bitmapinfoheader.biWidth * sizeof(UINT)); // copy next line of data to destination

        // advance pointers
        dest_ptr += lpitch;
        source_ptr += bitmap->bitmapinfoheader.biWidth;
    } // end for index_y

    (lpdds)->Unlock(NULL); // unlock the surface 
    return;
}
void Change_To_Client_Rect(LPRECT rect)
{
    RECT rect2;
    GetWindowRect(main_window_handle, &rect2);
    rect2.left += window_client_x0;
    rect2.top += window_client_y0;
    rect2.right = rect2.left + screen_width;
    rect2.bottom = rect2.top + screen_height;
    rect->left += rect2.left;
    rect->right += rect2.left;
    rect->top += rect2.top;
    rect->bottom += rect2.top;
    return;
}
int Draw_Text_GDI(char* text, int x, int y, COLORREF color, LPDIRECTDRAWSURFACE7 lpdds)
{
    HDC xdc;
    if (FAILED(lpdds->GetDC(&xdc)))
        return(0);
    SetTextColor(xdc, color);
    SetBkMode(xdc, TRANSPARENT);
    TextOut(xdc, x, y, text, strlen(text));
    lpdds->ReleaseDC(xdc);
    return(1);
}