#include <stdio.h>
#define AIDS_IMPLEMENTATION
#include "aids.h"

typedef char Node[4];

typedef struct {
    Node node;
    Aids_Array nodes;
} Graph_Item;

void parse_input(Aids_String_Slice buffer, Aids_Array *graph) {
    aids_array_init(graph, sizeof(Graph_Item));

    Aids_String_Slice line = {0};
    while (aids_string_slice_tokenize(&buffer, '\n', &line)) {
        aids_string_slice_trim(&line);

        Graph_Item item = {0};
        aids_array_init(&item.nodes, sizeof(Node));

        Aids_String_Slice token = {0};
        AIDS_ASSERT(aids_string_slice_tokenize(&line, ':', &token), "Expected a node");
        AIDS_ASSERT(token.len == 3, "Expected node to be 3 characters long");
        memcpy(item.node, token.str, token.len);

        aids_string_slice_trim(&line);
        while (aids_string_slice_tokenize(&line, ' ', &token)) {
            AIDS_ASSERT(token.len == 3, "Expected node to be 3 characters long");

            Node neighbor = {0};
            memcpy(neighbor, token.str, token.len);
            AIDS_ASSERT(aids_array_append(&item.nodes, &neighbor) == AIDS_OK, aids_failure_reason());
        }

        AIDS_ASSERT(aids_array_append(graph, &item) == AIDS_OK, aids_failure_reason());
    }
}

Aids_Result find_node_index(Aids_Array graph, Node node, size_t *index) {
    for (size_t i = 0; i < graph.count; i++) {
        Graph_Item *item = NULL;
        AIDS_ASSERT(aids_array_get(&graph, i, (void **)&item) == AIDS_OK, aids_failure_reason());
        if (strcmp(item->node, node) == 0) {
            *index = i;
            return AIDS_OK;
        }
    }

    return AIDS_ERR;
}

typedef struct {
    Node start;
    Node target;
    long count;
} Memo_Entry;

long memo_lookup(Aids_Array *memo, Node start, Node target) {
    for (size_t i = 0; i < memo->count; i++) {
        Memo_Entry *entry = NULL;
        AIDS_ASSERT(aids_array_get(memo, i, (void **)&entry) == AIDS_OK, aids_failure_reason());
        if (strcmp(entry->start, start) == 0 && strcmp(entry->target, target) == 0) {
            return entry->count;
        }
    }

    return -1;
}

void memo_insert(Aids_Array *memo, Node start, Node target, long count) {
    Memo_Entry entry = {0};
    strcpy(entry.start, start);
    strcpy(entry.target, target);
    entry.count = count;

    AIDS_ASSERT(aids_array_append(memo, &entry) == AIDS_OK, aids_failure_reason());
}

long bfs_all_paths_rec(Aids_Array graph, Node start, Node target, Aids_Array *memo) {
    if (strcmp(start, target) == 0) {
        return 1;
    }

    long cached = memo_lookup(memo, start, target);
    if (cached != -1) {
        return cached;
    }

    long result = 0;
    size_t item_index = 0;
    if (find_node_index(graph, start, &item_index) != AIDS_OK) {
        return 0;
    }

    Graph_Item *item = NULL;
    AIDS_ASSERT(aids_array_get(&graph, item_index, (void **)&item) == AIDS_OK, aids_failure_reason());

    for (size_t i = 0; i < item->nodes.count; i++) {
        Node *neighbor = NULL;
        AIDS_ASSERT(aids_array_get(&item->nodes, i, (void **)&neighbor) == AIDS_OK, aids_failure_reason());
        result += bfs_all_paths_rec(graph, *neighbor, target, memo);
    }

    memo_insert(memo, start, target, result);
    return result;
}

long bfs_all_paths(Aids_Array graph, Node start, Node target) {
    Aids_Array memo = {0};
    aids_array_init(&memo, sizeof(Memo_Entry));

    return bfs_all_paths_rec(graph, start, target, &memo);
}

void part1(Aids_Array graph) {
    Node start = "you";
    Node target = "out";

    Aids_Array queue = {0};
    aids_array_init(&queue, sizeof(Node));
    AIDS_ASSERT(aids_array_append(&queue, start) == AIDS_OK, aids_failure_reason());

    long path_count = bfs_all_paths(graph, start, target);

    printf("PART1: %ld\n", path_count);
}

void part2(Aids_Array graph) {
    long path_count = bfs_all_paths(graph, "svr", "fft")
        * bfs_all_paths(graph, "fft", "dac")
        * bfs_all_paths(graph, "dac", "out")
        + bfs_all_paths(graph, "svr", "dac")
        * bfs_all_paths(graph, "dac", "fft")
        * bfs_all_paths(graph, "fft", "out");

    printf("PART2: %ld\n", path_count);
}

int main() {
    Aids_String_Slice buffer = {0};
    aids_io_read(NULL, &buffer, "r");
    aids_string_slice_trim(&buffer);

    Aids_Array graph = {0};
    parse_input(buffer, &graph);

    part1(graph);
    part2(graph);

    return 0;
}
