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
    SDL_Texture *fileIcon;
    SDL_Texture *directoryIcon;
    SDL_Texture *ExecutableIcon;
    SDL_Texture *penguin;
    SDL_Texture *phrase;
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

    // create window and renderer
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);

    // initialize and perform rendering loop
    initialize(renderer);
    render(renderer);
    SDL_Event event;
    SDL_WaitEvent(&event);
    while (event.type != SDL_QUIT)
    {
        //render(renderer);
        SDL_WaitEvent(&event);
    }

    // clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

    

void initialize(SDL_Renderer *renderer, AppData *data_ptr) {
    data_ptr->font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 24);

    SDL_Surface *img_surf = IMG_Load("resrc/images/linux-penguin.png");
    data_ptr->penguin = SDL_CreateTextureFromSurface(renderer, img_surf);
    SDL_FreeSurface(img_surf);
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
    rect.x = 200;
    rect.y = 100;
    rect.w = 165;
    rect.h = 200;
    SDL_RenderCopy(renderer, data_ptr->penguin, NULL, &rect);

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

