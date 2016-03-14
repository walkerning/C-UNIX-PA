#ifndef _BINARY_TREE_H
#define _BINARY_TREE_H

#include <stdlib.h>

typedef struct _binary_tree {
  struct _binary_tree* left;
  struct _binary_tree* right;
} binary_tree;

// -- macros --
#define BINARY_ROOT(x) binary_tree x;

/**
 * tree_entry - get the struct for this entry
 * @ptr:the &binary_tree pointer.
 * @type:the type of the struct this is embedded in.
 * @member:the name of the binary_tree within the struct.
 */
#define tree_entry(ptr, type, member) \
  container_of(ptr, type, member)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 */
#define container_of(ptr, type, member) ({ \
      const typeof(((type*)0) -> member) *__mptr = (ptr); \
      (type*) ((char*)__mptr - offsetof(type, member));})


#define offsetof(type, member) \
  ((size_t) &((type *)0)->member)

#define binary_tree_for_each(x, y) binary_tree_for_each_infix(x, y)

/**
 *  binary_tree_for_each- Iterate over a binary tree. (infix-order)
 */
void binary_tree_for_each_infix(binary_tree* root, void (*func)(binary_tree*));

/**
 *  binary_tree_for_each_pre- Iterate over a binary tree. (pre-order)
 */
void binary_tree_for_each_pre(binary_tree* root, void (*func)(binary_tree*));

/**
 *  binary_tree_for_each_post- Iterate over a binary tree. (post-order)
 */
void binary_tree_for_each_post(binary_tree* root, void (*func)(binary_tree*));

#endif
