#include "binary_tree.h"

void binary_tree_for_each(binary_tree* root, void (*func)(binary_tree*)) {
  if (root == NULL) {
    return;
  }
  func(root -> left);
  func(root);
  func(root -> right);
}

void binary_tree_for_each_pre(binary_tree* root, void (*func)(binary_tree*)) {
  if (root == NULL) {
    return;
  }
  func(root);
  func(root -> left);
  func(root -> right);
}

void binary_tree_for_each_post(binary_tree* root, void (*func)(binary_tree*)) {
  if (root == NULL) {
    return;
  }
  func(root -> left);
  func(root -> right);
  func(root);
}
