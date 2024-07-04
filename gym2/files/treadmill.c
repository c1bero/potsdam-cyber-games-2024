#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

static const char *prompt_format = "Tilty the treadmill trainer.\n"
                                   "Today's highscore is %zu points by %s.\n"
                                   "[1] Start new training\n"
                                   "[2] Exit\n"
                                   "> ";

#define NUM_PITCHES (6ULL)
#define TRAINING_SECONDS (20ULL)

typedef struct training_plan {
    double pitches[NUM_PITCHES];
    size_t trainings;
    char *name;
    struct training_plan *next;
} training_plan_t;

typedef struct trainee {
    char *name;
    training_plan_t *training_plans;
} trainee_t;

typedef struct highscore {
    char *name;
    size_t score;
} highscore_t;

static highscore_t perrys_score = {
    "Perry the Platypwny",
    133713376942ULL,
};

static const highscore_t *highscore = &perrys_score;

static void treadmill(void);
static bool check_highscore(const trainee_t *trainee);
static void change_highscore(char *name, size_t score);
static size_t score(const trainee_t *trainee);

int main(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    while (!feof(stdin)) {
        char input_buffer[256];
        memset(input_buffer, 0, 256);
        printf(prompt_format, highscore->score, highscore->name);
        if (fgets(input_buffer, 256, stdin) == NULL) {
            continue;
        }
        unsigned long option = strtoul(input_buffer, NULL, 10);
        switch (option) {
        case 1:
            treadmill();
            break;
        case 2:
            puts("Exiting.\n");
            exit(EXIT_SUCCESS);
            break;
        default:
            puts("Invalid selection.\n");
            break;
        }
    }

    exit(EXIT_SUCCESS);
}

void treadmill(void) {
    printf("Welcome to your treadmill training. Ready to beat %s?\n", highscore->name);
    trainee_t *trainee = malloc(sizeof(trainee_t));
    trainee->training_plans = NULL;
    char buffer[2048] = {0};
    printf("But first, tell me your name\n> ");
    fgets(buffer, sizeof(buffer), stdin);
    size_t name_len = strlen(buffer);
    while (name_len > 0 && buffer[name_len - 1] == '\n') {
        buffer[name_len - 1] = '\0';
        name_len = strlen(buffer);
    }
    char *name = malloc(name_len + 1);
    if (name == NULL) {
        perror("malloc");
        return;
    }
    trainee->name = name;
    memcpy(trainee->name, buffer, name_len);
    trainee->name[name_len] = '\0';

    printf("Nice to meet you, %s\n", trainee->name);

    bool end = false;
    while(!feof(stdin) && !end) {
        printf("What do you want to do:\n\n[1] Create a training plan \n[2] List training plans\n[3] Delete a training plan\n[4] Train\n[5] Stop training\n> ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL || feof(stdin)) {
            continue;
        }
        size_t option = strtoul(buffer, NULL, 10);
        switch (option) {
        case 1: {
            printf("Please give your plan a name.\n> ");
            if (fgets(buffer, sizeof(buffer), stdin) == NULL || feof(stdin)) {
                continue;
            }
            name_len = strlen(buffer);
            while (name_len > 0 && buffer[name_len - 1] == '\n') {
                buffer[name_len - 1] = '\0';
                name_len = strlen(buffer);
            }

            name = malloc(name_len - 1);
            if (name == NULL) {
                perror("malloc");
                return;
            }

            training_plan_t *plan = (training_plan_t*)malloc(sizeof(training_plan_t));
            if (plan == NULL) {
                perror("malloc");
                return;
            }
            plan->name = name;
            plan->next = NULL;
            plan->trainings = 0;

            memcpy(plan->name, buffer, name_len);
            plan->name[name_len] = '\0';

            printf("How should it look like? Please enter 6 pitches.\n");
            for (size_t i = 0; i < NUM_PITCHES; ++i) {
                printf("Please provide pitch #%zu / %llu\n> ", i + 1, NUM_PITCHES);
                fgets(buffer, sizeof(buffer), stdin);
                plan->pitches[i] = strtod(buffer, NULL);
            }
            // Append plan.
            size_t i = 0;
            if (trainee->training_plans == NULL) {
                trainee->training_plans = plan;
            } else {
                training_plan_t *previous_plan = trainee->training_plans;
                ++i;
                while (previous_plan->next != NULL) {
                    previous_plan = previous_plan->next;
                    ++i;
                }
                previous_plan->next = plan;
            }
            printf("The number of your plan: %zu\n", i);
            break;
        }
        case 2: {
            if (trainee->training_plans == NULL) {
                printf("No plans available.\n");
                break;
            }
            training_plan_t *plan = trainee->training_plans;
            size_t i = 0;
            while (plan != NULL) {
                printf("Plan #%zu (%s) pitches:\n", i, plan->name);
                for (size_t p = 0; p < NUM_PITCHES; ++p) {
                    printf("  - %lf\n", plan->pitches[p]);
                }
                plan = plan->next;
                ++i;
            }
            printf("\n");
            break;
        }
        case 3: {
            printf("Which plan should be deleted? Enter the number.\n> ");
            fgets(buffer, sizeof(buffer), stdin);
            unsigned long number = strtoul(buffer, NULL, 10);
            size_t i = 0;
            training_plan_t *previous_plan = NULL;
            training_plan_t *plan = trainee->training_plans;
            while (plan->next != NULL && i < number) {
                previous_plan = plan;
                plan = plan->next;
                ++i;
            }
            if (i != number || plan == NULL) {
                printf("Plan not found.\n");
                break;
            }
            if(previous_plan != NULL) {
                previous_plan->next = plan->next;
            } else {
                trainee->training_plans = plan->next;
            }
            free(plan->name);
            plan->name = NULL;
            free(plan);
            printf("Plan deleted.\n");
            break;
        }
        case 4: {
            printf("Please enter the number of your training plan.\n> ");
            fgets(buffer, sizeof(buffer), stdin);
            unsigned long plan_number = strtoul(buffer, NULL, 10);

            training_plan_t *plan = trainee->training_plans;
            unsigned long i = 0;
            while (i < plan_number && plan != NULL) {
                plan = plan->next;
                ++i;
            }
            if (i != plan_number || plan == NULL) {
                printf("Could not find this plan.\n");
                break;
            }
            printf("Good choice!\nYou need to train for %lld seconds.\n", TRAINING_SECONDS * NUM_PITCHES);
            for (size_t p = 0; p < NUM_PITCHES; ++p) {
                printf("Now setting to pitch %f.\n", plan->pitches[p]);
                sleep(TRAINING_SECONDS);
            }
            plan->trainings++;
            printf("Wow! That was pretty good.\n");
            break;
        }
        case 5: {
            char *name_copy = malloc(strlen(trainee->name) + 1);
            memcpy(name_copy, trainee->name, strlen(trainee->name));
            name_copy[strlen(trainee->name)] = '\0';
            if(check_highscore(trainee)) {
                change_highscore(name_copy, score(trainee));
            } else {
                free(name_copy);
            }
            end = true;
            break;
        }
        default:
            printf("Invalid selection.\n");

        }
    }
end:
    training_plan_t *plan = trainee->training_plans;
    while(plan != NULL) {
        training_plan_t *tmp = plan->next;
        plan->next = NULL;
        free(plan->name);
        plan->name = NULL;
        free(plan);
        plan = tmp;
    }
    trainee->training_plans = NULL;
    free(trainee->name);
    trainee->name = NULL;
    free(trainee);
}

static size_t score(const trainee_t *trainee) {
    size_t score = 0;
    training_plan_t *current = trainee->training_plans;
    while (current != NULL) {
        score += current->trainings;
        current = current->next;
    }
    printf("Your score: %zu\n", score);
    return score;
}

static void print_reward() {
    const char *flag = getenv("FLAG");
    if (!flag) {
        printf("FLAG is missing.\n");
        return;
    }

    printf("Wow! You reached a new high score. Take this as little reward and present it at the gala dinner: %s\n", flag);
}

static void change_highscore(char *name, size_t score) {
    highscore_t *new_high_score = malloc(sizeof(new_high_score));
    new_high_score->name = name;
    new_high_score->score = score;
    highscore = new_high_score;
    print_reward();
}

static bool check_highscore(const trainee_t *trainee) {
    return score(trainee) > highscore->score;
}
