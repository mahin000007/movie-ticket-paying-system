#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct movie {
    char name[100];
    char type[50];
    char time[10];
};

struct ticket {
    char person[100];
    char film[100];
    int spots;
    char when[10];
};

struct ticketnode {
    struct ticket item;
    struct ticketnode* link;
};

struct comingsoon {
    char name[100];
    char date[20];
};

struct moviequeue {
    struct comingsoon* list;
    int start;
    int end;
    int max;
};

struct movietree {
    struct movie film;
    struct movietree* left;
    struct movietree* right;
};

struct recenttickets {
    struct ticket* list;
    int current;
    int max;
};

struct ticketnode* maketicketnode(struct ticket t) {
    struct ticketnode* new = malloc(sizeof(struct ticketnode));
    new->item = t;
    new->link = NULL;
    return new;
}

void saveticket(struct ticketnode** head, struct ticket t) {
    struct ticketnode* new = maketicketnode(t);
    if (*head == NULL) {
        *head = new;
        return;
    }
    struct ticketnode* now = *head;
    while (now->link != NULL) {
        now = now->link;
    }
    now->link = new;
}

void viewtickets(struct ticketnode* head) {
    if (head == NULL) {
        printf("nothing booked yet\n");
        return;
    }
    struct ticketnode* now = head;
    while (now != NULL) {
        printf("%s got %d seats for %s at %s\n",
               now->item.person,
               now->item.spots,
               now->item.film,
               now->item.when);
        now = now->link;
    }
}

void preparequeue(struct moviequeue* q, int size) {
    q->list = malloc(size * sizeof(struct comingsoon));
    q->start = -1;
    q->end = -1;
    q->max = size;
}

void queuemovie(struct moviequeue* q, struct comingsoon m) {
    if (q->end == q->max - 1) return;
    if (q->start == -1) q->start = 0;
    q->end++;
    q->list[q->end] = m;
}

struct comingsoon nextmovie(struct moviequeue* q) {
    if (q->start == -1) {
        struct comingsoon empty = {"", ""};
        return empty;
    }
    struct comingsoon m = q->list[q->start];
    q->start++;
    if (q->start > q->end) q->start = q->end = -1;
    return m;
}

void showcoming(struct moviequeue* q) {
    if (q->start == -1) {
        printf("nothing coming soon\n");
        return;
    }
    for (int i = q->start; i <= q->end; i++) {
        printf("%s (releases %s)\n", q->list[i].name, q->list[i].date);
    }
}

struct movietree* makemovienode(struct movie m) {
    struct movietree* new = malloc(sizeof(struct movietree));
    new->film = m;
    new->left = NULL;
    new->right = NULL;
    return new;
}

void addfilm(struct movietree** root, struct movie m) {
    if (*root == NULL) {
        *root = makemovienode(m);
        return;
    }
    if (strcmp(m.name, (*root)->film.name) < 0) {
        addfilm(&(*root)->left, m);
    } else {
        addfilm(&(*root)->right, m);
    }
}

void displayfilms(struct movietree* root) {
    if (root != NULL) {
        displayfilms(root->left);
        printf("%s (%s) showing at %s\n", root->film.name, root->film.type, root->film.time);
        displayfilms(root->right);
    }
}

struct movietree* locatefilm(struct movietree* root, char* name) {
    if (root == NULL) return NULL;
    int diff = strcmp(name, root->film.name);
    if (diff == 0) return root;
    if (diff < 0) return locatefilm(root->left, name);
    return locatefilm(root->right, name);
}

void setuprecent(struct recenttickets* s, int size) {
    s->list = malloc(size * sizeof(struct ticket));
    s->current = -1;
    s->max = size;
}

void addrecent(struct recenttickets* s, struct ticket t) {
    if (s->current == s->max - 1) return;
    s->current++;
    s->list[s->current] = t;
}

struct ticket seerecent(struct recenttickets* s) {
    if (s->current == -1) {
        struct ticket empty = {"", "", 0, ""};
        return empty;
    }
    return s->list[s->current];
}

void reserveticket(struct movietree* films, struct ticketnode** bookings, struct recenttickets* history) {
    displayfilms(films);
    printf("\nwhat movie? ");
    char name[100];
    fgets(name, 100, stdin);
    name[strcspn(name, "\n")] = 0;

    struct movietree* film = locatefilm(films, name);
    if (film == NULL) {
        printf("not found\n");
        return;
    }

    struct ticket new;
    printf("your name: ");
    fgets(new.person, 100, stdin);
    new.person[strcspn(new.person, "\n")] = 0;

    printf("how many seats? ");
    scanf("%d", &new.spots);
    getchar();

    strcpy(new.film, film->film.name);
    strcpy(new.when, film->film.time);

    saveticket(bookings, new);
    addrecent(history, new);

    printf("booked %d seats for %s at %s\n", new.spots, new.film, new.when);
}

void viewrecent(struct recenttickets* history) {
    struct ticket last = seerecent(history);
    if (strlen(last.person) == 0) {
        printf("no recent bookings\n");
        return;
    }
    printf("last booking: %s for %s at %s\n", last.person, last.film, last.when);
}

void loadexamples(struct movietree** films, struct moviequeue* upcoming) {
    struct movie examples[] = {
        {"the matrix", "sci-fi", "18:00"},
        {"inception", "sci-fi", "21:00"},
        {"the avengers", "action", "15:30"}
    };
    for (int i = 0; i < 3; i++) {
        addfilm(films, examples[i]);
    }

    struct comingsoon soon[] = {
        {"dune part 2", "2024-03-15"},
        {"deadpool 3", "2024-07-26"},
        {"joker 2", "2024-10-04"}
    };
    for (int i = 0; i < 3; i++) {
        queuemovie(upcoming, soon[i]);
    }
}

void showoptions() {
    printf("\nmovie ticket system\n");
    printf("1. whats playing\n");
    printf("2. book tickets\n");
    printf("3. my bookings\n");
    printf("4. last booking\n");
    printf("5. coming soon\n");
    printf("6. exit\n");
}

int main() {
    struct movietree* films = NULL;
    struct ticketnode* bookings = NULL;
    struct moviequeue upcoming;
    struct recenttickets history;

    preparequeue(&upcoming, 10);
    setuprecent(&history, 50);
    loadexamples(&films, &upcoming);

    int option;
    do {
        showoptions();
        printf("choose: ");
        scanf("%d", &option);
        getchar();

        switch (option) {
            case 1:
                displayfilms(films);
                break;
            case 2:
                reserveticket(films, &bookings, &history);
                break;
            case 3:
                viewtickets(bookings);
                break;
            case 4:
                viewrecent(&history);
                break;
            case 5:
                showcoming(&upcoming);
                break;
            case 6:
                printf("goodbye\n");
                break;
            default:
                printf("try again\n");
        }
    } while (option != 6);

    return 0;
}
