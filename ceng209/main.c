#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structs for Player and Room
typedef struct {
    int health;
    int strength;
    int inventory_capacity;
    char **inventory;
    int inventory_count;
} Player;

typedef struct Room {
    char *description;
    struct Room *up;
    struct Room *down;
    struct Room *left;
    struct Room *right;
    char *item;
    int has_creature;
    int creature_health;
} Room;

// Function prototypes
void initialize_player(Player *player);
Room *initialize_rooms();
void free_rooms(Room *start_room);
void move_player(Room **current_room, char *direction);
void look_room(Room *current_room);
void show_inventory(Player *player);
void pickup_item(Player *player, Room *current_room);
void attack_creature(Player *player, Room *current_room);
void save_game(Player *player, Room *rooms[], int room_count, Room *current_room, const char *filepath);
void load_game(Player *player, Room *rooms[], int room_count, Room **current_room, const char *filepath);

// Room indexing
#define ROOM_COUNT 4

int main() {
    Player player;
    initialize_player(&player);

    Room *rooms[ROOM_COUNT];
    rooms[0] = initialize_rooms();
    rooms[1] = rooms[0]->right;
    rooms[2] = rooms[1]->right;
    rooms[3] = rooms[2]->left;

    Room *current_room = rooms[0];

    char command[256];
    char arg[50];

    printf("Welcome to the game Way Out of the Mysterious Dungeons of the Egyptian Pyramids!\n");

    while (1) {
        printf("\n> ");
        scanf("%s", command);

        if (strcmp(command, "move") == 0) {
            scanf("%s", arg);
            move_player(&current_room, arg);
        } else if (strcmp(command, "look") == 0) {
            look_room(current_room);
        } else if (strcmp(command, "inventory") == 0) {
            show_inventory(&player);
        } else if (strcmp(command, "pickup") == 0) {
            pickup_item(&player, current_room);
        } else if (strcmp(command, "attack") == 0) {
            attack_creature(&player, current_room);
        }else if (strcmp(command, "menu") == 0) {
            show_the_menu();
        }else if (strcmp(command, "save") == 0) {
            scanf("%s", arg);
            save_game(&player, rooms, ROOM_COUNT, current_room, arg);
        } else if (strcmp(command, "load") == 0) {
            scanf("%s", arg);
            load_game(&player, rooms, ROOM_COUNT, &current_room, arg);
        } else if (strcmp(command, "exit") == 0) {
            printf("Exiting the game. Goodbye!\n");
            break;
        } else {
            printf("Invalid command. Try again.\n");
        }
    }

    free_rooms(rooms[0]);
    for (int i = 0; i < player.inventory_count; i++) {
        free(player.inventory[i]);
    }
    free(player.inventory);

    return 0;
}

void initialize_player(Player *player) {
    player->health = 100;
    player->strength = 10;
    player->inventory_capacity = 5;
    player->inventory = (char **)malloc(player->inventory_capacity * sizeof(char *));
    player->inventory_count = 0;
}

Room *initialize_rooms() {
    Room *room1 = (Room *)malloc(sizeof(Room));
    Room *room2 = (Room *)malloc(sizeof(Room));
    Room *room3 = (Room *)malloc(sizeof(Room));
    Room *room4 = (Room *)malloc(sizeof(Room));

    room1->description = strdup("You are in a pitch dark dungeon with no torches. There is a passageway to the east.");
    room1->item = strdup("torch with a big match");
    room1->up = NULL;
    room1->down = NULL;
    room1->left = NULL;
    room1->right = room2;
    room1->has_creature = 0;

    room2->description = strdup("You are in a corridor. There are doors to the west and east.");
    room2->item = strdup("Water");
    room2->up = NULL;
    room2->down = NULL;
    room2->left = room1;
    room2->right = room3;
    room2->has_creature = 0;

    room3->description = strdup("You are in a very small dungeon like a tunnel. There is a door to the west and up.");
    room3->item = strdup("Poisoned Tears Potion");
    room3->up = NULL;
    room3->down = NULL;
    room3->left = room2;
    room3->right = room4;
    room3->has_creature = 0;

    room4->description = strdup("You are in a huge dungeon with the sound of snoring ringing in your ears and you cannot see anything.");
    room4->item = strdup("An Arrow with A Dope Bow");
    room4->up = NULL;
    room4->down = NULL;
    room4->left = room3;
    room4->right = NULL;
    room4->has_creature = 1;
    room4->creature_health = 20;

    return room1;
}

void free_rooms(Room *start_room) {
    if (!start_room) return;
    free_rooms(start_room->up);
    free_rooms(start_room->down);
    free_rooms(start_room->left);
    free_rooms(start_room->right);

    free(start_room->description);
    free(start_room->item);
    free(start_room);
}

void move_player(Room **current_room, char *direction) {
    Room *next_room = NULL;

    if (strcmp(direction, "up") == 0) {
        next_room = (*current_room)->up;
    } else if (strcmp(direction, "down") == 0) {
        next_room = (*current_room)->down;
    } else if (strcmp(direction, "left") == 0) {
        next_room = (*current_room)->left;
    } else if (strcmp(direction, "right") == 0) {
        next_room = (*current_room)->right;
    } else {
        printf("Invalid direction. Use up, down, left, or right.\n");
        return;
    }

    if (next_room) {
        *current_room = next_room;
        printf("You moved %s.\n", direction);
    } else {
        printf("You can't move in that direction.\n");
    }
}

void look_room(Room *current_room) {
    printf("%s\n", current_room->description);

    if (current_room->item) {
        printf("You see a %s here.\n", current_room->item);
    }

    if (current_room->has_creature) {
        printf("There is a hostile creature here with a terrible and frightening image that feeds on your tears and consumes all your bodily fluids.TRY NOT TO CRY!!\n");
    }
}

void show_inventory(Player *player) {
    if (player->inventory_count == 0) {
        printf("Your inventory is empty.\n");
    } else {
        printf("Your inventory contains:\n");
        for (int i = 0; i < player->inventory_count; i++) {
            printf("- %s\n", player->inventory[i]);
        }
    }
}

void pickup_item(Player *player, Room *current_room) {
    if (!current_room->item) {
        printf("There is nothing to pick up here.\n");
        return;
    }

    if (player->inventory_count >= player->inventory_capacity) {
        printf("Your inventory is full.\n");
        return;
    }

    player->inventory[player->inventory_count] = strdup(current_room->item);
    player->inventory_count++;

    printf("You picked up a %s.\n", current_room->item);
    free(current_room->item);
    current_room->item = NULL;
}

void attack_creature(Player *player, Room *current_room) {
    if (!current_room->has_creature) {
        printf("There is no creature to attack here.\n");
        return;
    }

    while (player->health > 0 && current_room->creature_health > 0) {
        current_room->creature_health -= player->strength;
        printf("You hit the creature! Its health is now %d.\n", current_room->creature_health);

        if (current_room->creature_health <= 0) {
            printf("You defeated the creature!\n");
            current_room->has_creature = 0;
            break;
        }

        player->health -= 5;
        printf("The creature hits back! Your health is now %d.\n", player->health);

        if (player->health <= 0) {
            printf("You have been defeated by the creature. Game over.\n");
            exit(0);
        }
    }
}

void show_the_menu() {
    printf("Available commands:\n");
    printf(" - move <direction>\n");
    printf(" - look\n");
    printf(" - inventory\n");
    printf(" - pickup\n");
    printf(" - attack\n");
    printf(" - save <filename>\n");
    printf(" - load <filename>\n");
    printf(" - exit\n");
}

void save_game(Player *player, Room *rooms[], int room_count, Room *current_room, const char *filepath) {
    FILE *file = fopen(filepath, "w");
    if (!file) {
        printf("Failed to save the game.\n");
        return;
    }

    // Save player stats
    fprintf(file, "%d %d %d\n", player->health, player->strength, player->inventory_count);
    for (int i = 0; i < player->inventory_count; i++) {
        fprintf(file, "%s\n", player->inventory[i]);
    }

    // Save current room index
    for (int i = 0; i < room_count; i++) {
        if (rooms[i] == current_room) {
            fprintf(file, "%d\n", i);
            break;
        }
    }

    // Save room states
    for (int i = 0; i < room_count; i++) {
        fprintf(file, "%s\n", rooms[i]->description);
        fprintf(file, "%s\n", rooms[i]->item ? rooms[i]->item : "NULL");
        fprintf(file, "%d %d\n", rooms[i]->has_creature, rooms[i]->creature_health);
    }

    fclose(file);
    printf("Game saved successfully to %s.\n", filepath);
}

void load_game(Player *player, Room *rooms[], int room_count, Room **current_room, const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        printf("Failed to load the game.\n");
        return;
    }

    // Load player stats
    fscanf(file, "%d %d %d\n", &player->health, &player->strength, &player->inventory_count);

    // Load inventory
    for (int i = 0; i < player->inventory_count; i++) {
        char buffer[256];
        fscanf(file, "%255s\n", buffer);
        player->inventory[i] = strdup(buffer);
    }

    // Load current room index
    int current_room_index;
    fscanf(file, "%d\n", &current_room_index);
    *current_room = rooms[current_room_index];

    // Load room states
    for (int i = 0; i < room_count; i++) {
        char description[512];
        char item[256];
        int has_creature, creature_health;

        fscanf(file, "%511[^\n]\n", description);
        fscanf(file, "%255s\n", item);
        fscanf(file, "%d %d\n", &has_creature, &creature_health);

        free(rooms[i]->description);
        rooms[i]->description = strdup(description);

        if (strcmp(item, "NULL") != 0) {
            free(rooms[i]->item);
            rooms[i]->item = strdup(item);
        } else {
            free(rooms[i]->item);
            rooms[i]->item = NULL;
        }

        rooms[i]->has_creature = has_creature;
        rooms[i]->creature_health = creature_health;
    }

    fclose(file);
    printf("Game loaded successfully from %s.\n", filepath);
}
