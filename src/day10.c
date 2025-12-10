#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define AIDS_IMPLEMENTATION
#include "aids.h"

typedef struct {
    long *positions;
    size_t position_count;
} Button;

typedef struct {
    char *lights;
    size_t light_count;
    Aids_Array /* Button */ buttons;
    Aids_Array /* long */ jolts;
} Machine;

void parse_button(Aids_String_Slice line, Button *button) {
    aids_string_slice_trim(&line);

    Aids_Array positions = {0};
    aids_array_init(&positions, sizeof(long));

    Aids_String_Slice token = {0};
    while (aids_string_slice_tokenize(&line, ',', &token)) {
        aids_string_slice_trim(&token);
        long position = 0;
        AIDS_ASSERT(aids_string_slice_atol(&token, &position, 10), "Expected a number.");
        AIDS_ASSERT(aids_array_append(&positions, &position) == AIDS_OK, aids_failure_reason());
        aids_string_slice_trim(&line);
    }

    button->position_count = positions.count;
    button->positions = (long *)malloc(button->position_count * sizeof(long));
    for (size_t i = 0; i < positions.count; i++) {
        long *pos = NULL;
        AIDS_ASSERT(aids_array_get(&positions, i, (void **)&pos) == AIDS_OK, aids_failure_reason());
        button->positions[i] = *pos;
    }
    aids_array_free(&positions);
}

void parse_joltages(Aids_String_Slice line, Aids_Array *jolts) {
    aids_string_slice_trim(&line);

    Aids_String_Slice token = {0};
    while (aids_string_slice_tokenize(&line, ',', &token)) {
        aids_string_slice_trim(&token);
        long jolt = 0;
        AIDS_ASSERT(aids_string_slice_atol(&token, &jolt, 10), "Expected a number.");
        AIDS_ASSERT(aids_array_append(jolts, &jolt) == AIDS_OK, aids_failure_reason());
        aids_string_slice_trim(&line);
    }
}

void parse_machine(Aids_String_Slice line, Machine *machine) {
    // [.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
    AIDS_ASSERT(line.str[0] == '[', "Expected '[' at the beginning of machine definition.");
    aids_string_slice_skip(&line, 1);

    Aids_Array lights = {0};
    aids_array_init(&lights, sizeof(char));
    while (line.len > 0 && line.str[0] != ']') {
        char light = line.str[0];
        AIDS_ASSERT(aids_array_append(&lights, &light) == AIDS_OK, aids_failure_reason());
        aids_string_slice_skip(&line, 1);
    }
    AIDS_ASSERT(line.len > 0 && line.str[0] == ']', "Expected ']' at the end of lights definition.");
    aids_string_slice_skip(&line, 1);
    aids_string_slice_trim(&line);

    machine->light_count = lights.count;
    machine->lights = (char *)malloc(machine->light_count + 1);
    for (size_t i = 0; i < lights.count; i++) {
        char *light = NULL;
        AIDS_ASSERT(aids_array_get(&lights, i, (void **)&light) == AIDS_OK, aids_failure_reason());
        machine->lights[i] = *light;
    }
    aids_array_free(&lights);

    Aids_String_Slice token = {0};
    aids_array_init(&machine->buttons, sizeof(Button));
    while (line.len > 0) {
        aids_string_slice_trim(&line);
        if (line.str[0] != '(') {
            break;
        }

        Aids_String_Slice button_slice = {0};
        char *button_end = strchr((const char *)line.str, ')');
        AIDS_ASSERT(button_end != NULL, "Expected ')' in button definition.");
        button_slice.str = line.str + 1;
        button_slice.len = (unsigned long)(button_end - (char *)line.str - 1);

        Button button = {0};
        parse_button(button_slice, &button);
        AIDS_ASSERT(aids_array_append(&machine->buttons, &button) == AIDS_OK, aids_failure_reason());

        aids_string_slice_skip(&line, (unsigned long)(button_end - (char *)line.str) + 1);
    }

    aids_string_slice_trim(&line);
    aids_array_init(&machine->jolts, sizeof(long));
    if (line.len > 0 && line.str[0] == '{') {
        Aids_String_Slice jolt_slice = {0};
        char *jolt_end = strchr((const char *)line.str, '}');
        AIDS_ASSERT(jolt_end != NULL, "Expected '}' in joltages definition.");
        jolt_slice.str = line.str + 1;
        jolt_slice.len = (unsigned long)(jolt_end - (char *)line.str - 1);
        parse_joltages(jolt_slice, &machine->jolts);
    }
}

void parse_input(Aids_String_Slice buffer, Aids_Array *machines) {
    aids_array_init(machines, sizeof(Machine));

    Aids_String_Slice line = {0};
    while (aids_string_slice_tokenize(&buffer, '\n', &line)) {
        aids_string_slice_trim(&line);

        Machine machine = {0};
        parse_machine(line, &machine);

        AIDS_ASSERT(aids_array_append(machines, &machine) == AIDS_OK, aids_failure_reason());
    }
}

typedef struct {
    char *lights;
    size_t presses;
} State;

void state_find(Aids_Array /* State */ states, const char *lights, size_t lights_count, State **out_state) {
    for (size_t i = 0; i < states.count; i++) {
        State *state = NULL;
        AIDS_ASSERT(aids_array_get(&states, i, (void **)&state) == AIDS_OK, aids_failure_reason());

        if (strncmp(state->lights, lights, lights_count) == 0) {
            *out_state = state;
            return;
        }
    }

    *out_state = NULL;
}

size_t count_presses(Machine machine) {
    size_t result = 0;
    Aids_Array states = {0};
    aids_array_init(&states, sizeof(State));
    Aids_Array queue = {0};
    aids_array_init(&queue, sizeof(char) * (machine.light_count + 1));

    State initial_state = {0};
    initial_state.lights = (char *)malloc(machine.light_count + 1);
    memset(initial_state.lights, '.', machine.light_count);
    initial_state.presses = 0;

    AIDS_ASSERT(aids_array_append(&states, &initial_state) == AIDS_OK, aids_failure_reason());
    AIDS_ASSERT(aids_array_append(&queue, initial_state.lights) == AIDS_OK, aids_failure_reason());

    while (queue.count > 0) {
        char *current_lights = malloc(machine.light_count + 1);
        AIDS_ASSERT(aids_array_pop(&queue, 0, (void *)current_lights) == AIDS_OK, aids_failure_reason());

        if (strncmp(current_lights, machine.lights, machine.light_count) == 0) {
            State *state = NULL;
            state_find(states, current_lights, machine.light_count, &state);
            AIDS_ASSERT(state != NULL, "State not found.");
            AIDS_FREE(current_lights);
            return_defer(state->presses);
        }

        for (size_t i = 0; i < machine.buttons.count; i++) {
            Button *button = NULL;
            AIDS_ASSERT(aids_array_get(&machine.buttons, i, (void **)&button) == AIDS_OK, aids_failure_reason());

            char *new_lights = (char *)malloc(machine.light_count + 1);
            memcpy(new_lights, current_lights, machine.light_count);

            for (size_t j = 0; j < button->position_count; j++) {
                size_t pos = button->positions[j];
                if (pos < machine.light_count) {
                    new_lights[pos] = (new_lights[pos] == '#') ? '.' : '#';
                }
            }

            State *existing_state = NULL;
            state_find(states, new_lights, machine.light_count, &existing_state);
            if (existing_state == NULL) {
                State *current_state = NULL;
                state_find(states, current_lights, machine.light_count, &current_state);
                AIDS_ASSERT(current_state != NULL, "Current state not found.");

                State new_state = {0};
                new_state.lights = new_lights;
                new_state.presses = current_state->presses + 1;
                AIDS_ASSERT(aids_array_append(&states, &new_state) == AIDS_OK, aids_failure_reason());
                AIDS_ASSERT(aids_array_append(&queue, new_lights) == AIDS_OK, aids_failure_reason());
            } else {
                AIDS_FREE(new_lights);
            }
        }
    }

defer:
    return result;
}

void part1(Aids_Array machines) {
    size_t total_presses = 0;
    for (size_t i = 0; i < machines.count; i++) {
        Machine *machine = NULL;
        AIDS_ASSERT(aids_array_get(&machines, i, (void **)&machine) == AIDS_OK, aids_failure_reason());

        total_presses += count_presses(*machine);
    }

    printf("PART1: %zu\n", total_presses);
}

void part2(Aids_Array machines) {
    printf("PART2: TODO\n");
}

int main() {
    Aids_String_Slice buffer = {0};
    aids_io_read(NULL, &buffer, "r");
    aids_string_slice_trim(&buffer);

    Aids_Array machines = {0};
    parse_input(buffer, &machines);

    part1(machines);
    part2(machines);

    return 0;
}
