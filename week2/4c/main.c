#include <stdio.h>
#include "binary_tree.h"

struct my_node {
  int data;
  binary_tree tree;
};

int main() {
  BINARY_ROOT(my_tree);
  struct my_node *left_node = malloc(sizeof(struct my_node));
  left_node -> data = 1;
  struct my_node *right_node = malloc(sizeof(struct my_node));
  right_node -> data = 2;
  tree_set_left(&my_tree, &(left_node -> tree));
  tree_set_right(&my_tree, &(right_node -> tree));

  // test
  printf("left: %d\nright: %d\n", tree_entry(my_tree.left, struct my_node, tree) -> data,
         tree_entry(my_tree.right, struct my_node, tree) -> data);
}
