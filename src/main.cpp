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
#include <unistd.h>

//gnome-terminal -x bash -c "../ls

#define WIDTH 800
#define HEIGHT 600

enum icon_type {
    directory_icon, executable_icon, code_icon, image_icon, video_icon, other_icon
};

typedef struct drawItem {
    SDL_Rect icon_rect;
    SDL_Rect file_name_rect;
    SDL_Rect file_size_rect;
    SDL_Rect file_permissions_rect;
    SDL_Surface *file_name_surface;
    SDL_Texture *file_name_texture;
    SDL_Surface *file_size_surface;
    SDL_Texture *file_size_texture;
    SDL_Surface *file_permissions_surface;
    SDL_Texture *file_permissions_texture;
    icon_type type;
    std::string permissions;
    std::string fileNameAsString;
    std::string filePath;
    int indent;
} drawItem;

typedef struct AppData {
    //everytime you select a directory- destroy exisiting, create new text based
    //textures, every interaction after DO NOT RECREATE

    //With the rendering of textures, Don't draw the ones above and below, if y value is on
    //screen render, otherwise don't render.
    TTF_Font *font;
    SDL_Color text_color;
    SDL_Rect file_name_rect;
    SDL_Texture *icons[6];
    std::vector<drawItem*> file_list;
    SDL_Rect scroll_button_up;
    SDL_Rect scroll_button_down;
    SDL_Rect TurnRecursiveOn;
    bool icon_selected;
    bool file_name_selected;
    bool scroll_button_down_selected;
    bool scroll_button_up_selected;
    bool TurnRecursiveOn_selected;
    bool recursive_turned_on;
    SDL_Surface *scroll_button_up_sur;
    SDL_Texture *scroll_button_up_tex;
    SDL_Surface *scroll_button_down_sur;
    SDL_Texture *scroll_button_down_tex;
    SDL_Surface *TurnRecursiveOn_button_sur;
    SDL_Texture *TurnRecursiveOn_button_tex;
    SDL_Surface *TurnRecursiveOFF_button_sur;
    SDL_Texture *TurnRecursiveOFF_button_tex;
} AppData;

void listDirectoryRecursive(std::string dirname, std::vector<drawItem*>& file_list, SDL_Renderer *renderer, AppData *data_ptr, int indent);
void listDirectoryNon_Rec(std::string dirname, std::vector<drawItem*>& file_list, SDL_Renderer *renderer, AppData *data_ptr);
void initialize(SDL_Renderer *renderer, AppData *data_ptr);
void render(SDL_Renderer *renderer, AppData *data_ptr);
void quit(AppData *data_ptr);
std::string file_permissions(struct stat *file_info);
std::string sizeToString(double size);
void open_file(std::string name);
void open_exec(std::string name);

int n = 0;
int y = 40;



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

    data.recursive_turned_on = false;
    
    listDirectoryNon_Rec(home, data.file_list, renderer, &data);
    render(renderer, &data);

    

    while (event.type != SDL_QUIT) {
        //render(renderer);
        //listDirectoryNon_Rec(home, data.file_list, renderer, &data);
        SDL_WaitEvent(&event);
        

        switch (event.type) {
            //implement a button on the top and bottom not a scroll bar
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT && event.button.x >= data.scroll_button_up.x && event.button.x <= data.scroll_button_up.x + data.scroll_button_up.w && event.button.y >= data.scroll_button_up.y && event.button.y <= data.scroll_button_up.y + data.scroll_button_up.h) {
                    data.scroll_button_up_selected = true;
                    if(n-1 >= 0){
                        n = n-1;
                    }
                    if(data.recursive_turned_on){
                        listDirectoryNon_Rec(home, data.file_list, renderer, &data);
                        render(renderer, &data);
                    }else{
                        listDirectoryNon_Rec(home, data.file_list, renderer, &data);
                        render(renderer, &data);
                    }
                } else if (event.button.button == SDL_BUTTON_LEFT && event.button.x >= data.scroll_button_down.x && event.button.x <= data.scroll_button_down.x + data.scroll_button_down.w && event.button.y >= data.scroll_button_down.y && event.button.y <= data.scroll_button_down.y + data.scroll_button_down.h) {
                    data.scroll_button_down_selected = true;
                    
                    n = n+1;
                    if(data.recursive_turned_on){
                        listDirectoryNon_Rec(home, data.file_list, renderer, &data);
                    }else{
                        listDirectoryNon_Rec(home, data.file_list, renderer, &data);
                    }
                    if( data.file_list.size() >= 13){
                        render(renderer, &data);
                    }else{
                        n = n-1;
                        if(data.recursive_turned_on){
                            listDirectoryNon_Rec(home, data.file_list, renderer, &data);
                        }else{
                            listDirectoryNon_Rec(home, data.file_list, renderer, &data);
                        }
                        render(renderer, &data);
                    }
                } else if (event.button.button == SDL_BUTTON_LEFT && event.button.x >= data.TurnRecursiveOn.x && event.button.x <= data.TurnRecursiveOn.x + data.TurnRecursiveOn.w && event.button.y >= data.TurnRecursiveOn.y && event.button.y <= data.TurnRecursiveOn.y + data.TurnRecursiveOn.h) {
                    data.TurnRecursiveOn_selected = true;
                    if (data.recursive_turned_on) {
                        data.recursive_turned_on = false;
                        listDirectoryNon_Rec(home, data.file_list, renderer, &data);
                        render(renderer, &data);
                    } else {
                        data.recursive_turned_on = true;
                        listDirectoryNon_Rec(home, data.file_list, renderer, &data);
                        render(renderer, &data);
                    }
                } else {
                    for(int i = 0; i < data.file_list.size(); i++) {
                        if ((event.button.button == SDL_BUTTON_LEFT && event.button.x >= data.file_list[i]->icon_rect.x && event.button.x <= data.file_list[i]->icon_rect.x + data.file_list[i]->icon_rect.w && event.button.y >= data.file_list[i]->icon_rect.y && event.button.y <= data.file_list[i]->icon_rect.y + data.file_list[i]->icon_rect.h) 
                        || (event.button.button == SDL_BUTTON_LEFT && event.button.x >= data.file_list[i]->file_name_rect.x && event.button.x <= data.file_list[i]->file_name_rect.x + data.file_list[i]->file_name_rect.w && event.button.y >= data.file_list[i]->file_name_rect.y && event.button.y <= data.file_list[i]->file_name_rect.y + data.file_list[i]->file_name_rect.h)) {
                            data.icon_selected = true;
                            data.file_name_selected = true;
                            if (data.file_list[i]->type == directory_icon) {

                                
                                if (!(data.recursive_turned_on)) {
                                    if(data.file_list[i]->fileNameAsString == ".."){
                                        if(data.file_list[i]->filePath != "/home"){
                                            int index = data.file_list[i]->filePath.find_last_of('/');
                                            std::string newPath = data.file_list[i]->filePath.substr(0, index);
                                            strcpy(home, newPath.c_str());
                                            n=0;
                                            listDirectoryNon_Rec(newPath, data.file_list, renderer, &data);
                                            render(renderer, &data);
                                        }
                                    }else{
                                        std::string newPath = data.file_list[i]->filePath + "/" + data.file_list[i]->fileNameAsString;
                                        strcpy(home, newPath.c_str());
                                        n=0;
                                        listDirectoryNon_Rec(newPath, data.file_list, renderer, &data);
                                        render(renderer, &data);
                                    }
                                    
                                    
                                } else {

                                    if(data.file_list[i]->fileNameAsString == ".."){
                                        if(data.file_list[i]->filePath != "/home"){
                                            int index = data.file_list[i]->filePath.find_last_of('/');
                                            std::string newPath = data.file_list[i]->filePath.substr(0, index);
                                            strcpy(home, newPath.c_str());
                                            n=0;
                                            listDirectoryNon_Rec(home, data.file_list, renderer, &data);
                                            render(renderer, &data);
                                        }
                                    }else{
                                        std::string newPath = data.file_list[i]->filePath + "/" + data.file_list[i]->fileNameAsString;
                                        strcpy(home, newPath.c_str());
                                        n=0;
                                        listDirectoryNon_Rec(home, data.file_list, renderer, &data);
                                        render(renderer, &data);
                                    }


                                }
                            } else if (data.file_list[i]->type == executable_icon) {
                                std::string newPath = "../" + data.file_list[i]->fileNameAsString;
                                open_exec(newPath);
                                //gnome-terminal -- bash -c "../ls; exec bash"

                            } else {
                                std::string newPath = data.file_list[i]->filePath + "/" + data.file_list[i]->fileNameAsString;
                                open_file(newPath);  
                            }
                            break;
                        } 
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                data.scroll_button_up_selected = false;
                data.scroll_button_down_selected = false;
                data.TurnRecursiveOn_selected = false;
                data.icon_selected = false;
                data.file_name_selected = false;
                break;
        }

        //render(renderer, &data);
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
    data_ptr->text_color = { 0, 0, 0 };
    data_ptr->font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 16);


    SDL_Surface *img_surf = IMG_Load("resrc/directory_icon.png");
    data_ptr->icons[directory_icon] = SDL_CreateTextureFromSurface(renderer, img_surf);
    SDL_FreeSurface(img_surf);
    
    SDL_Surface *img_surf2 = IMG_Load("resrc/code_icon.png");
    data_ptr->icons[code_icon] = SDL_CreateTextureFromSurface(renderer, img_surf2);
    SDL_FreeSurface(img_surf2);

    SDL_Surface *img_surf3 = IMG_Load("resrc/other_icon.png");
    data_ptr->icons[other_icon] = SDL_CreateTextureFromSurface(renderer, img_surf3);
    SDL_FreeSurface(img_surf3);

    SDL_Surface *img_surf4 = IMG_Load("resrc/photo_icon.png");
    data_ptr->icons[image_icon] = SDL_CreateTextureFromSurface(renderer, img_surf4);
    SDL_FreeSurface(img_surf4);

    SDL_Surface *img_surf5 = IMG_Load("resrc/video_icon.png");
    data_ptr->icons[video_icon] = SDL_CreateTextureFromSurface(renderer, img_surf5);
    SDL_FreeSurface(img_surf5);

    SDL_Surface *img_surf6 = IMG_Load("resrc/executable_icon.png");
    data_ptr->icons[executable_icon] = SDL_CreateTextureFromSurface(renderer, img_surf6);
    SDL_FreeSurface(img_surf6);

    data_ptr->scroll_button_up_sur = TTF_RenderText_Solid(data_ptr->font, "Scroll up", data_ptr->text_color);
    data_ptr->scroll_button_up_tex = SDL_CreateTextureFromSurface(renderer, data_ptr->scroll_button_up_sur);
    SDL_FreeSurface(data_ptr->scroll_button_up_sur);

    data_ptr->scroll_button_down_sur = TTF_RenderText_Solid(data_ptr->font, "Scroll down", data_ptr->text_color);
    data_ptr->scroll_button_down_tex = SDL_CreateTextureFromSurface(renderer, data_ptr->scroll_button_down_sur);
    SDL_FreeSurface(data_ptr->scroll_button_down_sur);

    data_ptr->TurnRecursiveOn_button_sur = TTF_RenderText_Solid(data_ptr->font, "ON", data_ptr->text_color);
    data_ptr->TurnRecursiveOn_button_tex = SDL_CreateTextureFromSurface(renderer, data_ptr->TurnRecursiveOn_button_sur);
    SDL_FreeSurface(data_ptr->TurnRecursiveOn_button_sur);

    data_ptr->TurnRecursiveOFF_button_sur = TTF_RenderText_Solid(data_ptr->font, "OFF", data_ptr->text_color);
    data_ptr->TurnRecursiveOFF_button_tex = SDL_CreateTextureFromSurface(renderer, data_ptr->TurnRecursiveOFF_button_sur );
    SDL_FreeSurface(data_ptr->TurnRecursiveOFF_button_sur);

}

void render(SDL_Renderer *renderer, AppData *data_ptr) {
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 200, 255);
    data_ptr->scroll_button_up.x = 0;
    data_ptr->scroll_button_up.y = 0;
    data_ptr->scroll_button_up.w = 800;
    data_ptr->scroll_button_up.h = 30;
    SDL_RenderFillRect(renderer, &(data_ptr->scroll_button_up));
    
    data_ptr->scroll_button_down.x = 0;
    data_ptr->scroll_button_down.y = 570;
    data_ptr->scroll_button_down.w = 800;
    data_ptr->scroll_button_down.h = 30;
    SDL_RenderFillRect(renderer, &(data_ptr->scroll_button_down));
    if(data_ptr->recursive_turned_on){

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        data_ptr->TurnRecursiveOn.x = 0;
        data_ptr->TurnRecursiveOn.y = 250;
        data_ptr->TurnRecursiveOn.w =50;
        data_ptr->TurnRecursiveOn.h =50;
        SDL_RenderFillRect(renderer, &(data_ptr->TurnRecursiveOn));

    }else{
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        data_ptr->TurnRecursiveOn.x = 0;
        data_ptr->TurnRecursiveOn.y = 250;
        data_ptr->TurnRecursiveOn.w =50;
        data_ptr->TurnRecursiveOn.h =50;
        SDL_RenderFillRect(renderer, &(data_ptr->TurnRecursiveOn));
    }
    
    for(int i = 0; i < data_ptr->file_list.size(); i++) {
        //icon, name, size, value
        SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
        SDL_RenderCopy(renderer, data_ptr->icons[data_ptr->file_list[i]->type], NULL, &data_ptr->file_list[i]->icon_rect);
        SDL_QueryTexture(data_ptr->file_list[i]->file_name_texture, NULL, NULL, &(data_ptr->file_list[i]->file_name_rect.w),
        &(data_ptr->file_list[i]->file_name_rect.h));
        SDL_RenderCopy(renderer, data_ptr->file_list[i]->file_name_texture, NULL, &data_ptr->file_list[i]->file_name_rect);

        SDL_QueryTexture(data_ptr->file_list[i]->file_size_texture, NULL, NULL, &(data_ptr->file_list[i]->file_size_rect.w),
        &(data_ptr->file_list[i]->file_size_rect.h));
        SDL_RenderCopy(renderer, data_ptr->file_list[i]->file_size_texture, NULL, &data_ptr->file_list[i]->file_size_rect);

        SDL_QueryTexture(data_ptr->file_list[i]->file_permissions_texture, NULL, NULL, &(data_ptr->file_list[i]->file_permissions_rect.w),
        &(data_ptr->file_list[i]->file_permissions_rect.h));
        SDL_RenderCopy(renderer, data_ptr->file_list[i]->file_permissions_texture, NULL, &data_ptr->file_list[i]->file_permissions_rect);
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
    y = 40;

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
        for(int i = n+1; i < filenames.size(); i++) {
            if(y > 520){
                break;
            }
            err = stat((dirname + "/" + filenames[i]).c_str(), &file_info);
            if (err) {SDL_Surface *file_name_surface;
                SDL_Texture *file_name_texture;
                fprintf(stderr, "File does not exist");
            } else {
                if(S_ISDIR(file_info.st_mode) && filenames[i] != ".") {

                    drawItem *toPush = new drawItem();
                    //fill in fields toPush
                    SDL_Rect rect;
                    rect.x = 60;
                    rect.y = y;
                    rect.w = 30;
                    rect.h = 30;
                    toPush->icon_rect = rect;

                    toPush->type = directory_icon;

                    SDL_Rect name;
                    name.x = 100;
                    name.y = rect.y+5;
                    name.w = 60;
                    name.h = 30;

                    toPush->fileNameAsString = filenames[i].c_str();
                    toPush->filePath = dirname.c_str();
                    toPush->file_name_surface  = TTF_RenderText_Solid(data_ptr->font, filenames[i].c_str(), data_ptr->text_color);
                    toPush->file_name_texture = SDL_CreateTextureFromSurface(renderer, toPush->file_name_surface);
                    SDL_FreeSurface(toPush->file_name_surface);
                    toPush->file_name_rect = name;
                    
                    SDL_Rect size;
                    size.x = 300;
                    size.y = rect.y+5;
                    size.w = 60;
                    size.h = 30;

                    toPush->file_size_surface  = TTF_RenderText_Solid(data_ptr->font, "directory", data_ptr->text_color);
                    toPush->file_size_texture = SDL_CreateTextureFromSurface(renderer, toPush->file_size_surface);
                    SDL_FreeSurface(toPush->file_size_surface);
                    toPush->file_size_rect = size;

                    SDL_Rect permissions;
                    permissions.x = 400;
                    permissions.y = rect.y+5;
                    permissions.w = 60;
                    permissions.h = 30;

                    std::string permission = file_permissions(&file_info);

                    toPush->file_permissions_surface  = TTF_RenderText_Solid(data_ptr->font, permission.c_str(), data_ptr->text_color);
                    toPush->file_permissions_texture = SDL_CreateTextureFromSurface(renderer, toPush->file_permissions_surface);
                    SDL_FreeSurface(toPush->file_permissions_surface);
                    toPush->file_permissions_rect = permissions;                    

                    y = y + 40;
                    data_ptr->file_list.push_back(toPush);
                    
                    if(data_ptr->recursive_turned_on){
                        if (filenames[i] != "." && filenames[i] != "..") {
                            listDirectoryRecursive((dirname + "/" + filenames[i]), data_ptr->file_list, renderer, data_ptr, 50);
                        }
                    }
                    
                    
                } else {
                    drawItem *toPush = new drawItem();
                    //fill in fields toPush
                    SDL_Rect rect;
                    rect.x = 60;
                    rect.y = y;
                    rect.w = 30;
                    rect.h = 30;
                    toPush->icon_rect = rect;
                    SDL_Rect name;
                    name.x = 100;
                    name.y = rect.y+5;
                    name.w = 60;
                    name.h = 30;

                    std::string permission = file_permissions(&file_info);
                    toPush->fileNameAsString = filenames[i].c_str();
                    toPush->filePath = dirname.c_str();

                    std::string tempString = filenames[i].c_str();
                    if(tempString.size() > 20){
                        tempString = tempString.substr(0, 20) + "...";
                    }

                    toPush->file_name_surface  = TTF_RenderText_Solid(data_ptr->font, tempString.c_str(), data_ptr->text_color);
                    toPush->file_name_texture = SDL_CreateTextureFromSurface(renderer, toPush->file_name_surface);
                    SDL_FreeSurface(toPush->file_name_surface);
                    toPush->file_name_rect = name;

                    SDL_Rect size;
                    size.x = 300;
                    size.y = rect.y+5;
                    size.w = 60;
                    size.h = 30;

                    double size_int = file_info.st_size;
                    toPush->file_size_surface  = TTF_RenderText_Solid(data_ptr->font, sizeToString(size_int).c_str(), data_ptr->text_color);
                    toPush->file_size_texture = SDL_CreateTextureFromSurface(renderer, toPush->file_size_surface);
                    SDL_FreeSurface(toPush->file_size_surface);
                    toPush->file_size_rect = size;

                    SDL_Rect permissions;
                    permissions.x = 400;
                    permissions.y = rect.y+5;
                    permissions.w = 60;
                    permissions.h = 30;

                    
                    

                    toPush->file_permissions_surface  = TTF_RenderText_Solid(data_ptr->font, permission.c_str(), data_ptr->text_color);
                    toPush->file_permissions_texture = SDL_CreateTextureFromSurface(renderer, toPush->file_permissions_surface);
                    SDL_FreeSurface(toPush->file_permissions_surface);
                    toPush->file_permissions_rect = permissions;


                    int find = filenames[i].find('.');
                    if(find < 0){
                        find = 0;
                    }
                    std::string file_extension = filenames[i].substr(find);
                      
                    if(file_extension == ".jpg" || file_extension == ".jpeg" || file_extension == ".png" || file_extension == ".tif" 
                    || file_extension == ".tiff" || file_extension == ".gif") {

                        toPush->type = image_icon;
                        y = y + 40;
                        data_ptr->file_list.push_back(toPush);
                    }else if(file_extension == ".mp4" || file_extension == ".mov" || file_extension == ".mkv" || file_extension == ".avi" 
                    || file_extension == ".webm") {
                        toPush->type = video_icon;
                        y = y + 40;
                        data_ptr->file_list.push_back(toPush);
                    }else if(file_extension == ".h" || file_extension == ".c" || file_extension == ".cpp" || file_extension == ".py" 
                    || file_extension == ".java" || file_extension == ".js") {
                        toPush->type = code_icon;
                        y = y + 40;
                        data_ptr->file_list.push_back(toPush);
                    }else if ((S_IEXEC & file_info.st_mode) != 0 && filenames[i] != "."){
                        //Have to check if it's S_IXUSR or S_IEXEC later **
                        toPush->type = executable_icon;
                        y = y + 40;
                        data_ptr->file_list.push_back(toPush);
                    }else {
                        if(filenames[i] != "."){
                            toPush->type = other_icon;
                            y = y + 40;
                            data_ptr->file_list.push_back(toPush);
                        }
                    }

                }// end of else statement (not a directory)
            }// end of else statement (File exists)
        }//end of for loop
        closedir(dir);
    }
    else
    {
        fprintf(stderr, "Error: directory '%s' not found\n", dirname.c_str());
    }   
}



void listDirectoryRecursive(std::string dirname, std::vector<drawItem*>& file_list, SDL_Renderer *renderer, AppData *data_ptr, int indent){

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
        for(int i = n+1; i < filenames.size(); i++) {
            if(y > 520){
                break;
            }
            err = stat((dirname + "/" + filenames[i]).c_str(), &file_info);
            if (err) {SDL_Surface *file_name_surface;
                SDL_Texture *file_name_texture;
                fprintf(stderr, "File does not exist");
            } else {
                if(S_ISDIR(file_info.st_mode) && filenames[i] != "." && filenames[i] != "..") {

                    drawItem *toPush = new drawItem();
                    //fill in fields toPush
                    SDL_Rect rect;
                    rect.x = 60 + indent;
                    rect.y = y;
                    rect.w = 30;
                    rect.h = 30;
                    toPush->icon_rect = rect;

                    toPush->type = directory_icon;

                    SDL_Rect name;
                    name.x = 100 + indent;
                    name.y = rect.y+5;
                    name.w = 60;
                    name.h = 30;

                    toPush->fileNameAsString = filenames[i].c_str();
                    toPush->filePath = dirname.c_str();
                    toPush->file_name_surface  = TTF_RenderText_Solid(data_ptr->font, filenames[i].c_str(), data_ptr->text_color);
                    toPush->file_name_texture = SDL_CreateTextureFromSurface(renderer, toPush->file_name_surface);
                    SDL_FreeSurface(toPush->file_name_surface);
                    toPush->file_name_rect = name;
                    
                    SDL_Rect size;
                    size.x = 300 + indent;
                    size.y = rect.y+5;
                    size.w = 60;
                    size.h = 30;

                    toPush->file_size_surface  = TTF_RenderText_Solid(data_ptr->font, "directory", data_ptr->text_color);
                    toPush->file_size_texture = SDL_CreateTextureFromSurface(renderer, toPush->file_size_surface);
                    SDL_FreeSurface(toPush->file_size_surface);
                    toPush->file_size_rect = size;

                    SDL_Rect permissions;
                    permissions.x = 400 + indent;
                    permissions.y = rect.y+5;
                    permissions.w = 60;
                    permissions.h = 30;

                    std::string permission = file_permissions(&file_info);

                    toPush->file_permissions_surface  = TTF_RenderText_Solid(data_ptr->font, permission.c_str(), data_ptr->text_color);
                    toPush->file_permissions_texture = SDL_CreateTextureFromSurface(renderer, toPush->file_permissions_surface);
                    SDL_FreeSurface(toPush->file_permissions_surface);
                    toPush->file_permissions_rect = permissions; 
                                 
                    data_ptr->file_list.push_back(toPush);
                    y = y + 40;

                    if(data_ptr->recursive_turned_on){
                        if (filenames[i] != "." && filenames[i] != "..") {
                            listDirectoryRecursive((dirname + "/" + filenames[i]), data_ptr->file_list, renderer, data_ptr, indent +40);
                        }
                    }           
                    
                } else {

                    
                    drawItem *toPush = new drawItem();
                    //fill in fields toPush
                    SDL_Rect rect;
                    rect.x = 60 + indent;
                    rect.y = y;
                    rect.w = 30;
                    rect.h = 30;
                    toPush->icon_rect = rect;
                    SDL_Rect name;
                    name.x = 100 + indent;
                    name.y = rect.y+5;
                    name.w = 60;
                    name.h = 30;

                    std::string permission = file_permissions(&file_info);

                    toPush->fileNameAsString = filenames[i].c_str();
                    std::string tempString = filenames[i].c_str();
                    toPush->filePath = dirname.c_str();
                    if(tempString.size() > 20){
                        tempString = tempString.substr(0, 20) + "...";
                    }

                    toPush->file_name_surface  = TTF_RenderText_Solid(data_ptr->font, tempString.c_str(), data_ptr->text_color);
                    toPush->file_name_texture = SDL_CreateTextureFromSurface(renderer, toPush->file_name_surface);
                    SDL_FreeSurface(toPush->file_name_surface);
                    toPush->file_name_rect = name;

                    SDL_Rect size;
                    size.x = 300 + indent;
                    size.y = rect.y+5;
                    size.w = 60;
                    size.h = 30;

                    double size_int = file_info.st_size;
                    toPush->file_size_surface  = TTF_RenderText_Solid(data_ptr->font, sizeToString(size_int).c_str(), data_ptr->text_color);
                    toPush->file_size_texture = SDL_CreateTextureFromSurface(renderer, toPush->file_size_surface);
                    SDL_FreeSurface(toPush->file_size_surface);
                    toPush->file_size_rect = size;

                    SDL_Rect permissions;
                    permissions.x = 400 + indent;
                    permissions.y = rect.y+5;
                    permissions.w = 60;
                    permissions.h = 30;

                    
                    

                    toPush->file_permissions_surface  = TTF_RenderText_Solid(data_ptr->font, permission.c_str(), data_ptr->text_color);
                    toPush->file_permissions_texture = SDL_CreateTextureFromSurface(renderer, toPush->file_permissions_surface);
                    SDL_FreeSurface(toPush->file_permissions_surface);
                    toPush->file_permissions_rect = permissions;


                    int find = filenames[i].find('.');
                    if(find < 0){
                        find = 0;
                    }
                    std::string file_extension = filenames[i].substr(find);
                      
                    if(file_extension == ".jpg" || file_extension == ".jpeg" || file_extension == ".png" || file_extension == ".tif" 
                    || file_extension == ".tiff" || file_extension == ".gif") {

                        toPush->type = image_icon;
                        y = y + 40;
                        data_ptr->file_list.push_back(toPush);
                    }else if(file_extension == ".mp4" || file_extension == ".mov" || file_extension == ".mkv" || file_extension == ".avi" 
                    || file_extension == ".webm") {
                        toPush->type = video_icon;
                        y = y + 40;
                        data_ptr->file_list.push_back(toPush);
                    }else if(file_extension == ".h" || file_extension == ".c" || file_extension == ".cpp" || file_extension == ".py" 
                    || file_extension == ".java" || file_extension == ".js") {
                        toPush->type = code_icon;
                        y = y + 40;
                        data_ptr->file_list.push_back(toPush);
                    }else if ((S_IEXEC & file_info.st_mode) != 0 && filenames[i] != "." && filenames[i] != ".."){
                        //Have to check if it's S_IXUSR or S_IEXEC later **
                        toPush->type = executable_icon;
                        y = y + 40;
                        data_ptr->file_list.push_back(toPush);
                    }else {
                        if(filenames[i] != "." && filenames[i] != ".."){
                            toPush->type = other_icon;
                            y = y + 40;
                            data_ptr->file_list.push_back(toPush);
                        }
                    }

                }// end of else statement (not a directory)
            }// end of else statement (File exists)
        }//end of for loop
        indent = indent +40;
        closedir(dir);
    }
    else
    {
        fprintf(stderr, "Error: directory '%s' not found\n", dirname.c_str());
    }   
}


std::string file_permissions(struct stat *file_info)
{
    
    std::string permissions = "";
    //if file is a directory
    if(S_ISDIR(file_info->st_mode)){
        permissions = permissions + "d";
    }else{
        permissions = permissions + "-";
    }

    //File owner has read permission
    if(S_IRUSR & file_info->st_mode){
        permissions = permissions + "r";
    }else{
        permissions = permissions + "-";
    }
    //File owner has write permission
    if(S_IWUSR & file_info->st_mode){
        permissions = permissions + "w";
    }else{
        permissions = permissions + "-";
    }
    //File owner has execute/search permission
    if(S_IXUSR & file_info->st_mode){
        permissions = permissions + "x";
    }else{
        permissions = permissions + "-";
    }

    //The file's user group has read permission
    if(S_IRGRP & file_info->st_mode){
        permissions = permissions + "r";
    }else{
        permissions = permissions + "-";
    }
    //The file's user group has write permission
    if(S_IWGRP & file_info->st_mode){
        permissions = permissions + "w";
    }else{
        permissions = permissions + "-";
    }
    //The file's user group has execute/search permission
    if(S_IXGRP & file_info->st_mode){
        permissions = permissions + "x";
    }else{
        permissions = permissions + "-";
    }

    //Other users have read permission
    if(S_IROTH & file_info->st_mode){
        permissions = permissions + "r";
    }else{
        permissions = permissions + "-";
    }
    //Other users have write permission
    if(S_IWOTH & file_info->st_mode){
        permissions = permissions + "w";
    }else{
        permissions = permissions + "-";
    }
    //Other users have execute/search permission
    if(S_IXOTH & file_info->st_mode){
        permissions = permissions + "x";
    }else{
        permissions = permissions + "-";
    }

    return permissions;
}

std::string sizeToString(double size){

    //Gb
    if(size >= 1073741824){
        std::string size_string = std::to_string(size/1073741824);
        std::string rounded = size_string.substr(0, size_string.find(".")+2) + " GiB";
        return rounded;
    }else if(size >= 1048576){
        std::string size_string = std::to_string(size/1048576);
        std::string rounded = size_string.substr(0, size_string.find(".")+2) + " MiB";
        return rounded;
    }else if(size >= 1024){
        std::string size_string = std::to_string(size/1024);
        std::string rounded = size_string.substr(0, size_string.find(".")+2) + " kiB";
        return rounded;
    }else{
        std::string size_string = std::to_string(size) + "B";
        std::string rounded = size_string.substr(0, size_string.find(".")) + " B";
        return rounded;
    }

}





void open_file(std::string name){
    int pid = fork();
    if (pid == 0) {
        execl("/usr/bin/xdg-open", "xdg-open", name.c_str(), NULL);
    }
}

void open_exec(std::string name){
    int pid = fork();
    //gnome-terminal -x bash -c "../ls"

    if (pid == 0) {
        execl("/usr/bin/gnome-terminal", "gnome-terminal", "--",  "bash", "-c", (name + "; exec bash").c_str(), NULL);
    }
}