#include <string.h>
#include <wlr/backend/session.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/util/box.h>

#include "globals.h"
#include "layer.h"
#include "client.h"
#include "server.h"

void 
key_function(struct simple_server *server, struct keymap *keymap) 
{
   //--- QUIT -----
   if(keymap->keyfn==QUIT)    wl_display_terminate(server->display);

   //--- SPAWN -----
   if(keymap->keyfn==SPAWN)   spawn(keymap->argument);

   //--- TAG -----
   if(keymap->keyfn==TAG){
      if(!strcmp(keymap->argument, "prev"))     setCurrentTag(server, /*curtag*/-1, false);
      if(!strcmp(keymap->argument, "next"))     setCurrentTag(server, /*curtag*/+1, false);
      if(!strcmp(keymap->argument, "select"))   setCurrentTag(server, keymap->keysym-XKB_KEY_1, false);
      if(!strcmp(keymap->argument, "toggle"))   setCurrentTag(server, keymap->keysym-XKB_KEY_1, true);
      if(!strcmp(keymap->argument, "tile"))     tileTag(server);

      arrange_output(server->cur_output);
   }

   //--- CLIENT -----
   struct wlr_surface *surface = server->seat->keyboard_state.focused_surface;
   struct simple_client* client;
   int type = get_client_from_surface(surface, &client, NULL);

   say(DEBUG, "type = %d", type);
   if(keymap->keyfn==CLIENT) {
      if(!strcmp(keymap->argument, "cycle"))          cycleClients(server->cur_output);

      if(type<0) return;
      if(!strcmp(keymap->argument, "send_to_tag"))    sendClientToTag(client, keymap->keysym-XKB_KEY_1);
      if(!strcmp(keymap->argument, "toggle_fixed"))   toggleClientFixed(client);
      if(!strcmp(keymap->argument, "toggle_visible")) toggleClientVisible(client);
      if(!strcmp(keymap->argument, "kill"))           killClient(client);
      if(!strcmp(keymap->argument, "maximize"))       maximizeClient(client);
      if(!strcmp(keymap->argument, "tile_left"))      tileClient(client, LEFT);
      if(!strcmp(keymap->argument, "tile_right"))     tileClient(client, RIGHT);
      if(!strcmp(keymap->argument, "move")){
         if(keymap->keysym==XKB_KEY_Left)    client->geom.x-=server->config->moveresize_step;
         if(keymap->keysym==XKB_KEY_Right)   client->geom.x+=server->config->moveresize_step;
         if(keymap->keysym==XKB_KEY_Up)      client->geom.y-=server->config->moveresize_step;
         if(keymap->keysym==XKB_KEY_Down)    client->geom.y+=server->config->moveresize_step;
         set_client_geometry(client, client->geom);
      }
      if(!strcmp(keymap->argument, "resize")){
         if(keymap->keysym==XKB_KEY_Left)    client->geom.width-=server->config->moveresize_step;
         if(keymap->keysym==XKB_KEY_Right)   client->geom.width+=server->config->moveresize_step;
         if(keymap->keysym==XKB_KEY_Up)      client->geom.height-=server->config->moveresize_step;
         if(keymap->keysym==XKB_KEY_Down)    client->geom.height+=server->config->moveresize_step;
         set_client_geometry(client, client->geom);
      }
      // ...
      arrange_output(server->cur_output);
   }
}

void 
mouse_function(struct simple_client *client, struct mousemap *mousemap, int resize_edges)
{
   say(DEBUG, "mouse_function");
   if(mousemap->context==CONTEXT_ROOT){
      if(!strcmp(mousemap->argument, "test"))   say(INFO, "test()");
   }
   if(mousemap->context==CONTEXT_CLIENT){
      if(!client) return;
      if(!strcmp(mousemap->argument, "move"))   begin_interactive(client, CURSOR_MOVE, 0);
      if(!strcmp(mousemap->argument, "resize")) begin_interactive(client, CURSOR_RESIZE, resize_edges);
   }
}
