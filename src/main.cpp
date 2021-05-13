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

typedef struct AppData {
    //everytime you select a directory- destroy exisiting, create new text based
    //textures, every interaction after DO NOT RECREATE

    //With the rendering of textures, Don't draw the ones above and below, if y value is on
    //screen render, otherwise don't render.
    TTF_Font *font;
    SDL_Texture *code_icon;
    SDL_Texture *directory_icon;
    SDL_Texture *executable_icon;
    SDL_Texture *image_icon;
    SDL_Texture *video_icon;
    SDL_Texture *other_icon;
    SDL_Rect *code_icon_location;
    SDL_Rect *directory_icon_location;
    SDL_Rect *executable_icon_location;
    SDL_Rect *image_icon_location;
    SDL_Rect *video_icon_location;
    SDL_Rect *other_icon_location;
    SDL_Texture *code_filename;
    SDL_Texture *directory_name;
    SDL_Texture *executable_filename;
    SDL_Texture *image_filename;
    SDL_Texture *video_filename;
    SDL_Texture *other_filename;
    SDL_Rect *code_filename_location;
    SDL_Rect *directory_name_location;
    SDL_Rect *executable_filename_location;
    SDL_Rect *image_filename_location;
    SDL_Rect *video_filename_location;
    SDL_Rect *other_filename_location;

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
    
    SDL_Rect penguin_location;
    SDL_Rect phrase_location;
    bool penguin_selected;
    bool phrase_selected;
    SDL_Point penguin_offset;
} AppData;

void listDirectoryNon_Rec(std::string dirname);
void initialize(SDL_Renderer *renderer);
void render(SDL_Renderer *renderer);

int main(int argc, char **argv)
{
    char *home = getenv("HOME");
    printf("HOME: %s\n", home);

    // initializing SDL as Video
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_PNG);

    // create window and renderer
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);

    // initialize and perform rendering loop
    initialize(renderer);
    render(renderer);
    SDL_Event event;
    SDL_WaitEvent(&event);
    while (event.type != SDL_QUIT) {
        //render(renderer);
         SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_MOUSEMOTION:
                if (data.directory_icon_selected || data.directory_name_selected) {
                    data.phrase_location.x =  event.motion.x - data.phrase_offset.x;
                    data.phrase_location.y =  event.motion.y - data.phrase_offset.y;
                } else if (data.executable_icon_selected || data.executable_name_selected) {

                } else if (data.image_icon_selected || data.image_filename_selected) {

                } else if (data.video_icon_selected || data.video_filename_selected) {

                } else if (data.code_icon_selected || data.code_filename_selected) {

                } else if (data.other_icon_selected || data.other_filename_selected) {

                }
                break;
            case SDL_MOUSEBUTTONDOWN:
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
                        
                } 
                break;
            case SDL_MOUSEBUTTONUP:
                break;
        }

        render(renderer, &data);
    }

    // clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
    return 0;
}

    

void initialize(SDL_Renderer *renderer, AppData *data_ptr) {
    data_ptr->font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 24);

    SDL_Surface *img_surf = IMG_Load("resrc/images/directory_icon.png");
    data_ptr->directory_icon = SDL_CreateTextureFromSurface(renderer, img_surf);
    SDL_FreeSurface(img_surf);

    SDL_Surface *img_surf2 = IMG_Load("resrc/images/code_icon.png");
    data_ptr->code_icon = SDL_CreateTextureFromSurface(renderer, img_surf2);
    SDL_FreeSurface(img_surf2);

    SDL_Surface *img_surf3 = IMG_Load("resrc/images/other_icon.png");
    data_ptr->other_icon = SDL_CreateTextureFromSurface(renderer, img_surf3);
    SDL_FreeSurface(img_surf);

    SDL_Surface *img_surf4 = IMG_Load("resrc/images/image_icon.png");
    data_ptr->image_icon = SDL_CreateTextureFromSurface(renderer, img_surf4);
    SDL_FreeSurface(img_surf4);

    SDL_Surface *img_surf5 = IMG_Load("resrc/images/video_icon.png");
    data_ptr->video_icon = SDL_CreateTextureFromSurface(renderer, img_surf5);
    SDL_FreeSurface(img_surf5);

    SDL_Surface *img_surf6 = IMG_Load("resrc/images/directory_icon.png");
    data_ptr->directory_icon = SDL_CreateTextureFromSurface(renderer, img_surf6);
    SDL_FreeSurface(img_surf6);


    data_ptr->penguin_location.x = 200;
    data_ptr->penguin_location.y = 100;
    data_ptr->penguin_location.w = 165;
    data_ptr->penguin_location.h = 200;

    SDL_Color color = { 0, 0, 0 };
    SDL_Surface *phrase_surf = TTF_RenderText_Solid(data_ptr->font, "Hello World!", color);
    data_ptr->phrase = SDL_CreateTextureFromSurface(renderer, phrase_surf);
    SDL_FreeSurface(phrase_surf);
}

void render(SDL_Renderer *renderer, AppData *data_ptr) {
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderClear(renderer);
    
    // TODO: draw!
    SDL_Rect rect;
    rect.x = 10;
    rect.y = 10;
    rect.w = 10;
    rect.h = 10;
    SDL_RenderCopy(renderer, data_ptr->directory_icon, NULL, &rect);

    rect.x = 400;
    rect.y = 300;
    SDL_RenderCopy(renderer, data_ptr->penguin, NULL, &rect);

    SDL_QueryTexture(data_ptr->phrase, NULL, NULL, &(rect.w), &(rect.h));
    rect.x = 10;
    rect.y = 500;
    SDL_RenderCopy(renderer, data_ptr->phrase, NULL, &rect);

    // show rendered frame
    SDL_RenderPresent(renderer);
}

void quit(AppData *data_ptr) {
    SDL_DestroyTexture(data_ptr->penguin);
    SDL_DestroyTexture(data_ptr->phrase);
    TTF_CloseFont(data_ptr->font);
}

void listDirectoryNon_Rec(std::string dirname) {
    struct stat info;
    int err = stat(dirname.c_str(), &info);
    if (err == 0 && S_ISDIR(info.st_mode)) {
        DIR* dir = opendir(dirname.c_str());
        // TODO: modify to be able to print all entries in directory in alphabetical order
        //       in addition to file name, also print file size (or 'directory' if entry is a folder)
        //       Example output:
        //         ls.cpp (693 bytes)
        //         my_file.txt (62 bytes)
        //         OS Files (directory)
        std::vector<std::string> filenames;
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            filenames.push_back(entry->d_name);
        }
        std::sort(filenames.begin(), filenames.end());
        struct stat file_info;
        for(int i = 0; i < filenames.size(); i++){
            err = stat(dirname + "/" + filenames[i], &file_info);
            if (err) {
                fprintf(stderr, "File does not exist");
            } else {
                if(S_ISDIR(file_info.st_mode)){
                    printf("%s (Directory)\n", filenames[i].c_str());
                } else {
                    printf("%s (%ld bytes)\n", filenames[i].c_str(), file_info.st_size);
                }
            }
        }
        closedir(dir);
    }
    else
    {
        fprintf(stderr, "Error: directory '%s' not found\n", dirname.c_str());
    }
}

