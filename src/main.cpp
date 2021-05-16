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
    SDL_Surface *file_name_surface;
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
    SDL_Rect scroll_button_up;
    SDL_Rect scroll_button_down;
    SDL_Rect TurnRecursiveOn;
    bool icon_selected;
    bool filename_selected;
    bool scroll_button_down_selected;
    bool scroll_button_up_selected;
    bool TurnRecursiveOn_selected;
    bool recursive_turned_on;
    /*
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
    bool other_filename_selected;*/
} AppData;

void listDirectoryRecursive(std::string dirname, std::vector<drawItem*>& file_list, SDL_Renderer *renderer, AppData *data_ptr, int indent);
void listDirectoryNon_Rec(std::string dirname, std::vector<drawItem*>& file_list, SDL_Renderer *renderer, AppData *data_ptr);
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
    TTF_Init();
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
        listDirectoryNon_Rec(home, data.file_list, renderer, &data);
        SDL_WaitEvent(&event);


        switch (event.type) {
            //implement a button on the top and bottom not a scroll bar
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
    TTF_Quit();
    return 0;
}

    

void initialize(SDL_Renderer *renderer, AppData *data_ptr) {
    //Creates text
    SDL_Color text_color = { 0, 0, 0 };
    data_ptr->font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 14);
    for (int i = 0; i < data_ptr->file_list.size(); i++) {
        data_ptr->file_list[i]->file_name_texture = SDL_CreateTextureFromSurface(renderer, data_ptr->file_list[i]->file_name_surface);
    }
    
    
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
    SDL_SetRenderDrawColor(renderer, 255, 0, 200, 255);
    data_ptr->scroll_button_up.x = 770;
    data_ptr->scroll_button_up.y = 0;
    data_ptr->scroll_button_up.w = 30;
    data_ptr->scroll_button_up.h = 30;
    SDL_RenderFillRect(renderer, &(data_ptr->scroll_button_up));
    data_ptr->scroll_button_down.x = 770;
    data_ptr->scroll_button_down.y = 570;
    data_ptr->scroll_button_down.w = 30;
    data_ptr->scroll_button_down.h = 30;
    SDL_RenderFillRect(renderer, &(data_ptr->scroll_button_down));
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    data_ptr->TurnRecursiveOn.x = 0;
    data_ptr->TurnRecursiveOn.y = 540;
    data_ptr->TurnRecursiveOn.w =60;
    data_ptr->TurnRecursiveOn.h =60;
    SDL_RenderFillRect(renderer, &(data_ptr->TurnRecursiveOn));

    for(int i = 0; i < data_ptr->file_list.size(); i++) {
        //icon, name, size, value
       SDL_QueryTexture(data_ptr->file_list[i]->file_name_texture, NULL, NULL, &(data_ptr->file_list[i]->file_name_rect.w), &(data_ptr->file_list[i]->file_name_rect.h));
       SDL_RenderCopy(renderer, data_ptr->file_list[i]->file_name_texture, &(data_ptr->file_list[i]->file_name_rect), &(data_ptr->file_list[i]->icon_rect));
    }
    // show rendered frame
    SDL_RenderPresent(renderer);
}

void quit(AppData *data_ptr) {
    SDL_DestroyTexture(data_ptr->icons[directory_icon]);
    SDL_DestroyTexture(data_ptr->icons[code_icon]);
    SDL_DestroyTexture(data_ptr->icons[other_icon]);
    SDL_DestroyTexture(data_ptr->icons[image_icon]);
    SDL_DestroyTexture(data_ptr->icons[video_icon]);
    SDL_DestroyTexture(data_ptr->icons[executable_icon]);
    //TODO Question: Do we need to destroy our text textures by using a for loop?
    TTF_CloseFont(data_ptr->font);
}

void listDirectoryNon_Rec(std::string dirname, std::vector<drawItem*>& file_list, SDL_Renderer *renderer, AppData *data_ptr) {
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
        //Declared here to ensure items don't render on top of eachother
        int y = 20;
        for(int i = 0; i < filenames.size(); i++) {
            err = stat((dirname + "/" + filenames[i]).c_str(), &file_info);
            if (err) {
                fprintf(stderr, "File does not exist");
            } else {
                if(S_ISDIR(file_info.st_mode)) {
                    drawItem *toPush = new drawItem();
                    //fill in fields toPush
                    toPush->file_name_rect.x = 0;
                    toPush->file_name_rect.y = y;
                    //TODO Question: what to set for file_name_text? 
                    //Look at video (the portion for hello world)
                    SDL_Color text_color = { 0, 0, 0 };
                    toPush->file_name_surface  = TTF_RenderText_Solid(data_ptr->font, filenames[i].c_str(), text_color);
                    toPush->file_name_texture = SDL_CreateTextureFromSurface(renderer, toPush->file_name_surface);
                    SDL_FreeSurface(toPush->file_name_surface);
                    toPush->type = directory_icon; 
                    toPush->icon_rect.x = 0;
                    toPush->icon_rect.y = y;
                    toPush->icon_rect.w = 40;
                    toPush->icon_rect.h = 30;
                    file_list.push_back(toPush);
                } else {
                    //TODO: Alex implement here if/elses to properly assign file type
                    //find the loction of the last dot then take the substring from the dot to the rest of the string
                    //how you fill fields will be different
                    drawItem *toPush = new drawItem();
                    SDL_Color text_color = { 0, 0, 0 };
                    toPush->file_name_surface  = TTF_RenderText_Solid(data_ptr->font, filenames[i].c_str(), text_color);
                    toPush->file_name_texture = SDL_CreateTextureFromSurface(renderer, toPush->file_name_surface);
                    SDL_FreeSurface(toPush->file_name_surface);
                    toPush->file_name_rect.x = 0;
                    toPush->file_name_rect.y = y;
                    toPush->file_name_rect.w = 40;
                    toPush->file_name_rect.h = 30;
                    toPush->icon_rect.x = 0;
                    toPush->icon_rect.y = y;
                    toPush->icon_rect.w = 40;
                    toPush->icon_rect.h = 30;
                    for(int i=0; i < file_list.size(); i++){

                        std::string file_extension = filenames[i].substr(filenames[i].find('.'));

                        if(file_extension == ".jpg" || file_extension == ".jpeg" || file_extension == ".png" || file_extension == ".tif" 
                        || file_extension == ".tiff" || file_extension == ".gif") {

                            toPush->type = image_icon;

                        }else if(file_extension == ".mp4" || file_extension == ".mov" || file_extension == ".mkv" || file_extension == ".avi" 
                        || file_extension == ".webm") {

                            toPush->type = video_icon;

                        }else if(file_extension == ".h" || file_extension == ".c" || file_extension == ".cpp" || file_extension == ".py" 
                        || file_extension == ".java" || file_extension == ".js") {

                            toPush->type = video_icon;

                        }else {

                            toPush->type = other_icon;

                        }

                    }
                   
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



void listDirectoryRecursive(std::string dirname, std::vector<drawItem*>& file_list, SDL_Renderer *renderer, AppData *data_ptr, int indent = 0) {
    for (int i = 0; i < file_list.size(); i++) {
        SDL_DestroyTexture(file_list[i]->file_name_texture);
    }
    file_list.clear();
    std::string space = "";
    for (int i = 0; i < indent; i++) {
        space += " ";
    }
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
        //Declared here to ensure items don't render on top of eachother
        int y = 20;
        for(int i = 0; i < filenames.size(); i++) {
            err = stat((dirname + "/" + filenames[i]).c_str(), &file_info);
            if (err) {
                fprintf(stderr, "File does not exist");
            } else {
                if(S_ISDIR(file_info.st_mode)) {
                    drawItem *toPush = new drawItem();
                    //fill in fields toPush
                    toPush->file_name_rect.x = 0;
                    toPush->file_name_rect.y = y;
                    //TODO Question: what to set for file_name_text? 
                    //Look at video (the portion for hello world)
                    SDL_Color text_color = { 0, 0, 0 };
                    toPush->file_name_surface  = TTF_RenderText_Solid(data_ptr->font, filenames[i].c_str(), text_color);
                    toPush->file_name_texture = SDL_CreateTextureFromSurface(renderer, toPush->file_name_surface);
                    SDL_FreeSurface(toPush->file_name_surface);
                    toPush->type = directory_icon; 
                    toPush->icon_rect.x = 0;
                    toPush->icon_rect.y = y;
                    toPush->icon_rect.w = 40;
                    toPush->icon_rect.h = 30;
                    file_list.push_back(toPush);
                    //One line of code to 
                    if (filenames[i] != "." && filenames[i] != "..") {
                         listDirectoryRecursive((dirname + "/" + filenames[i]), data_ptr->file_list, renderer, data_ptr, (indent+1));
                    }       
                } else {
                    drawItem *toPush = new drawItem();
                    SDL_Color text_color = { 0, 0, 0 };
                    toPush->file_name_surface  = TTF_RenderText_Solid(data_ptr->font, filenames[i].c_str(), text_color);
                    toPush->file_name_texture = SDL_CreateTextureFromSurface(renderer, toPush->file_name_surface);
                    SDL_FreeSurface(toPush->file_name_surface);
                    toPush->file_name_rect.x = 0;
                    toPush->file_name_rect.y = y;
                    toPush->file_name_rect.w = 40;
                    toPush->file_name_rect.h = 30;
                    toPush->icon_rect.x = 0;
                    toPush->icon_rect.y = y;
                    toPush->icon_rect.w = 40;
                    toPush->icon_rect.h = 30;
                    for(int i=0; i < file_list.size(); i++){
                        std::string file_extension = filenames[i].substr(filenames[i].find('.'));
                        if(file_extension == ".jpg" || file_extension == ".jpeg" || file_extension == ".png" || file_extension == ".tif" 
                        || file_extension == ".tiff" || file_extension == ".gif") {

                            toPush->type = image_icon;

                        }else if(file_extension == ".mp4" || file_extension == ".mov" || file_extension == ".mkv" || file_extension == ".avi" 
                        || file_extension == ".webm") {

                            toPush->type = video_icon;

                        }else if(file_extension == ".h" || file_extension == ".c" || file_extension == ".cpp" || file_extension == ".py" 
                        || file_extension == ".java" || file_extension == ".js") {

                            toPush->type = video_icon;

                        }else {

                            toPush->type = other_icon;

                        }

                    }
                   
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

