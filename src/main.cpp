#include <iostream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define WIDTH 800
#define HEIGHT 600

enum icon_type {
    directory_icon, executable_icon, code_icon, image_icon, video_icon, other_icon
};

typedef struct drawItem {
    SDL_Rect file_name_rect;
    SDL_Texture *file_name_texture;
    icon_type type; 
    SDL_Rect icon_rect; 
} drawItem;

typedef struct AppData {
    //everytime you select a directory- destroy exisiting, create new text based
    //textures, every interaction after DO NOT RECREATE

    //With the rendering of textures, Don't draw the ones above and below, if y value is on
    //screen render, otherwise don't render.
    TTF_Font *font;
    SDL_Texture *icons[6];
    std::vector<drawItem*> file_list;
    bool code_icon_selected;
    bool directory_icon_selected;
    bool executable_icon_selected;
    bool image_icon_selected;
    bool video_icon_selected;
    bool other_icon_selected;
    bool code_filename_selected;
    bool directory_name_selected;
    bool executable_filename_selected;
    bool image_filename_selected;
    bool video_filename_selected;
    bool other_filename_selected;
} AppData;

void listDirectoryNon_Rec(std::string dirname, std::vector<drawItem*>& file_list);
void initialize(SDL_Renderer *renderer, AppData *data_ptr);
void render(SDL_Renderer *renderer, AppData *data_ptr);
void quit(AppData *data_ptr);

int main(int argc, char **argv)
{
    char *home = getenv("HOME");
    printf("HOME: %s\n", home);

    // initializing SDL as Video
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    // create window and renderer
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);

    // initialize and perform rendering loop
    AppData data;
    initialize(renderer, &data);
    render(renderer, &data);
    SDL_Event event;
    SDL_WaitEvent(&event);
    while (event.type != SDL_QUIT) {
        //render(renderer);
        listDirectoryNon_Rec(home, data->file_list);
         SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_MOUSEMOTION:
                //implement a button on the top and bottom not a scroll bar
                /*
                if (data.directory_icon_selected || data.directory_name_selected) {
                    data.phrase_location.x =  event.motion.x - data.phrase_offset.x;
                    data.phrase_location.y =  event.motion.y - data.phrase_offset.y;
                } else if (data.executable_icon_selected || data.executable_name_selected) {

                } else if (data.image_icon_selected || data.image_filename_selected) {

                } else if (data.video_icon_selected || data.video_filename_selected) {

                } else if (data.code_icon_selected || data.code_filename_selected) {

                } else if (data.other_icon_selected || data.other_filename_selected) {

                }*/
                break;
            case SDL_MOUSEBUTTONDOWN:
                /*
                if (event.button.x >= data.directory_icon_location.x && data.directory_icon_location.x + data.directory_icon_location.w && event.button.x >= data.directory_icon_location.y && data.directory_icon_location.y + data.directory_icon_location.h) {
                        data.directory_icon_selected = true;
                        //data.phrase_offset.x = event.button.x -data.phrase_location.x;
                        //data.phrase_offset.y = event.button.y - data.phrase_location.y;
                } else if (event.button.x >= data.directory_name_location.x && data.directory_name_location.x + data.directory_name_location.w && event.button.x >= data.directory_name_location.y && data.directory_name_location.y + data.directory_name_location.h)  {
                        data.directory_name_selected = true;
                        
                } else if (event.button.x >= data.code_icon_location.x && data.code_icon_location.x + data.code_icon_location.w && event.button.x >= data.code_icon_location.y && data.code_icon_location.y + data.code_icon_location.h)  {
                        data.directory_name_selected = true;
                        
                } else if (event.button.x >= data.code_filename_location.x && data.code_filename_location.x + data.code_filename_location.w && event.button.x >= data.code_filename_location.y && data.code_filename_location.y + data.code_filename_location.h)  {
                        data.directory_name_selected = true;
                        
                } */
                break;
            case SDL_MOUSEBUTTONUP:
                break;
        }

        render(renderer, &data);
    }

    // clean up
    quit(&data);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
    return 0;
}

    

void initialize(SDL_Renderer *renderer, AppData *data_ptr) {
    data_ptr->font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 14);

    SDL_Surface *img_surf = IMG_Load("resrc/images/directory_icon.png");
    data_ptr->icons[directory_icon] = SDL_CreateTextureFromSurface(renderer, img_surf);
    SDL_FreeSurface(img_surf);
    
    SDL_Surface *img_surf2 = IMG_Load("resrc/images/code_icon.png");
    data_ptr->icons[code_icon] = SDL_CreateTextureFromSurface(renderer, img_surf2);
    SDL_FreeSurface(img_surf2);

    SDL_Surface *img_surf3 = IMG_Load("resrc/images/other_icon.png");
    data_ptr->icons[other_icon] = SDL_CreateTextureFromSurface(renderer, img_surf3);
    SDL_FreeSurface(img_surf);

    SDL_Surface *img_surf4 = IMG_Load("resrc/images/image_icon.png");
    data_ptr->icons[image_icon] = SDL_CreateTextureFromSurface(renderer, img_surf4);
    SDL_FreeSurface(img_surf4);

    SDL_Surface *img_surf5 = IMG_Load("resrc/images/video_icon.png");
    data_ptr->icons[video_icon] = SDL_CreateTextureFromSurface(renderer, img_surf5);
    SDL_FreeSurface(img_surf5);

    SDL_Surface *img_surf6 = IMG_Load("resrc/images/executable_icon.png");
    data_ptr->icons[executable_icon] = SDL_CreateTextureFromSurface(renderer, img_surf6);
    SDL_FreeSurface(img_surf6);

}

void render(SDL_Renderer *renderer, AppData *data_ptr) {
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderClear(renderer); 
    for(int i = 0; i < data_ptr->file_list.size(); i++) {
        //icon, name, size, value
        SDL_RenderCopy(renderer, data_ptr->file_list[i]->file_name_texture, &(data_ptr->file_list[i]->file_name_rect), &(data_ptr->file_list[i]->icon_rect));
    }

    // show rendered frame
    SDL_RenderPresent(renderer);
}

void quit(AppData *data_ptr) {
    //TODO Question: How to destory the textures properly?
    SDL_DestroyTexture(data_ptr->icons[directory_icon]);
    SDL_DestroyTexture(data_ptr->icons[code_icon]);
    SDL_DestroyTexture(data_ptr->icons[other_icon]);
    SDL_DestroyTexture(data_ptr->icons[image_icon]);
    SDL_DestroyTexture(data_ptr->icons[video_icon]);
    SDL_DestroyTexture(data_ptr->icons[executable_icon]);
    TTF_CloseFont(data_ptr->font);
}

void listDirectoryNon_Rec(std::string dirname, std::vector<drawItem*>& file_list) {
    for (int i = 0; i < file_list.size(); i++) {
        SDL_DestroyTexture(file_list[i]->file_name_texture);
    }
    file_list.clear();
    struct stat info;
    int err = stat(dirname.c_str(), &info);
    if (err == 0 && S_ISDIR(info.st_mode)) {
        DIR* dir = opendir(dirname.c_str());
        std::vector<std::string> filenames;
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            filenames.push_back(entry->d_name);
        }
        std::sort(filenames.begin(), filenames.end());
        struct stat file_info;
        int y = 20;
        for(int i = 0; i < filenames.size(); i++) {
            err = stat((dirname + "/" + filenames[i]).c_str(), &file_info);
            if (err) {
                fprintf(stderr, "File does not exist");
            } else {
                if(S_ISDIR(file_info.st_mode)){
                    drawItem *toPush = new drawItem();
                    //fill in fields toPush
                    toPush->file_name_rect.x = 0;
                    toPush->file_name_rect.y = y;
                    toPush->file_name_rect.w = 40;
                    toPush->file_name_rect.h = 30;
                    //TODO Question: what to set here? 
                    //Look at the portion for hello world
                    toPush->file_name_texture ;
                    toPush->type = directory_icon; 
                    toPush->icon_rect.x = 0;
                    toPush->icon_rect.y = y;
                    toPush->icon_rect.w = 40;
                    toPush->icon_rect.h = 30;
                    file_list.push_back(toPush);
                } else {
                    //TODO Question: How to figure out what file type is to properly assign? Since all lengths are different between .jpeg vs .cpp vs .gif
                    //find the loction of the last dot then take the substring from the dot to the rest of the string
                    //how you fill fields will be different
                    drawItem *toPush = new drawItem();
                    toPush->file_name_rect.x = 0;
                    toPush->file_name_rect.y = y;
                    toPush->file_name_rect.w = 40;
                    toPush->file_name_rect.h = 30;
                    toPush->icon_rect.x = 0;
                    toPush->icon_rect.y = y;
                    toPush->icon_rect.w = 40;
                    toPush->icon_rect.h = 30;
                    //fill in fields toPush
                    file_list.push_back(toPush);
                }
                y += 10;
            }
        }
        closedir(dir);
    }
    else
    {
        fprintf(stderr, "Error: directory '%s' not found\n", dirname.c_str());
    }
}

