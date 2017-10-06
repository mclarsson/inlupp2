#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tree.h"

typedef struct node node_t;

struct tree {
  node_t *top;
  int size;
};

struct node {
  K key;
  T element;
  node_t *left;
  node_t *right;
};

enum key_compare { KEYS_MATCH, MOVE_RIGHT, MOVE_LEFT };

typedef struct {
  int index;
  enum { KEYS, ELEMENTS } type;
  union {
    K *keys;
    T *elements;
  };
} node_clt;

/// Creates a new tree
///
/// \returns: empty tree
tree_t *tree_new()
{
  return calloc(1, sizeof(tree_t));
}

/// Frees node and subtrees
///
/// \param node Node to start remove at
void free_branches(node_t *node, tree_action cleanup)
{
  if (node->left != NULL)
    {
      free_branches(node->left, cleanup);
    }

  if (node->right != NULL)
    {
      free_branches(node->right, cleanup);
    }

  cleanup(node->key, node->element);
}

/// Remove a tree along with all T elements.
///
/// \param tree the tree
/// \param cleanup a function that takes a key and element as
///        argument, to be used to free memory. If this param is 
///        NULL, no cleanup of keys or elements will happen.
void tree_delete(tree_t *tree, tree_action cleanup)
{
  if (tree->top != NULL)
    {
      free_branches(tree->top, cleanup);
    }

  free(tree);
}

/// Get the size of the tree 
///
/// \returns: the number of nodes in the tree
int tree_size(tree_t *tree)
{
  return tree->size;
}

/// Return the biggest of two numbers
///
/// \param a First number
/// \param b Second number
/// \returns Biggest number
int max(int a, int b)
{
  return a > b ? a : b;
}

/// Count max node depth
///
/// \param node current node
/// \returns depth of tree
int count_depth(node_t *node)
{
  if (node->left == NULL && node->right == NULL)
    {
      return 1;
    }
  else
    {
      int left  = node->left  == NULL ? 1 : 1 + count_depth(node->left);
      int right = node->right == NULL ? 1 : 1 + count_depth(node->right);
      return max(left, right);
    }
}

/// Get the depth of the tree 
///
/// \param tree tree to check
/// \returns: the depth of the deepest subtree
int tree_depth(tree_t *tree)
{
  return tree->top == NULL ? 0 : count_depth(tree->top);
}

/// Returns which direction to move in tree
/// based on parent and child key
///
/// \param node key of node
/// \param key key to compare with
/// \returns direction to move in
enum key_compare compare_keys(K node, K key)
{
  int a = strcmp(node, key);

  if (a == 0)     return KEYS_MATCH;
  else if (a < 0) return MOVE_RIGHT;
  else            return MOVE_LEFT;
}

/// Finds appropriate position for key in tree, or node
/// if key is in tree.
///
/// \param tree tree to search in
/// \param key key to search tree for
/// \returns double pointer to correct position
node_t **search_tree(tree_t *tree, K key)
{
  node_t **node = &(tree->top);
  
  while (*node != NULL)
    {
      int compare_result = compare_keys((*node)->key, key);
	
      if      (compare_result == KEYS_MATCH) return node;
      else if (compare_result == MOVE_LEFT)  node = &(*node)->left;
      else if (compare_result == MOVE_RIGHT) node = &(*node)->right;
    }

  // Reached end of branch, key not in tree
  return node;
}

/// Traverses all nodes in tree
///
/// \param node current node
/// \param order the order in which the elements will be visited
/// \param fun the function to apply to all elements
/// \param data an extra argument passed to each call to fun (may be NULL)
void traverse_tree(node_t *node, enum tree_order order, tree_action2 fun, void *data)
{
  if (node != NULL)
    {
      // PRE ORDER: handle node before both branches
      if (order == preorder) fun(node->key, node->element, data);

      // traverse left branch
      traverse_tree(node->left, order, fun, data);

      // IN ORDER: handle node between left and right branches
      if (order == inorder) fun(node->key, node->element, data);

      // traverse right branch
      traverse_tree(node->right, order, fun, data);

      // POST ORDER: handle node after both branches
      if (order == postorder) fun(node->key, node->element, data);
    }
}

/// Applies a function to all elements in the tree in a specified order.
/// Example (using shelf as key):
///
///     tree_t *t = tree_new();
///     tree_insert(t, "A25", some_item);
///     int number = 0;
///     tree_apply(t, inorder, print_item, &number);
///
/// where print_item is a function that prints the number and increments it,
/// and prints the item passed to it. 
///
/// \param tree the tree
/// \param order the order in which the elements will be visited
/// \param fun the function to apply to all elements
/// \param data an extra argument passed to each call to fun (may be NULL)
void tree_apply(tree_t *tree, enum tree_order order, tree_action2 fun, void *data)
{
  traverse_tree(tree->top, order, fun, data);
}

/// Insert element into the tree. Returns false if the key is already used.
///
/// \param tree pointer to the tree
/// \param key the key of element to be appended
/// \param elem the element 
/// \returns: true if successful, else false
bool tree_insert(tree_t *tree, K key, T elem)
{
  node_t *new = calloc(1, sizeof(node_t));
  new->key = strdup(key);
  new->element = elem;

  node_t **leaf = search_tree(tree, key);

  if (*leaf == NULL)
    {
      *leaf = new;
      ++tree->size;
      return true;
    }
  else
    {
      return false;
    }
}

/// Checks whether a key is used in a tree
///
/// \param tree pointer to the tree
/// \param key the key of elem to be removed
/// \returns: true if key is a key in tree
bool tree_has_key(tree_t *tree, K key)
{
  return *(search_tree(tree, key)) == NULL;
}

/// Returns the element for a given key in tree.
/// (The implementation may assume that the key exists.)
/// 
/// \param tree pointer to the tree
/// \param key the key of elem to be removed
/// \returns: true if key is a key in tree
T tree_get(tree_t *tree, K key)
{
  return (*search_tree(tree, key))->element;
}

/// Adds key or elem to node_clt, tree_action2 for tree_elements
/// and tree_keys
///
/// \param key key of node
/// \param elem element of node
/// \param data node_clt to add to
void collect_nodes(K key, T elem, void *data)
{
  node_clt *clt = data;
  
  if (clt->type == KEYS)
    {
      clt->keys[clt->index] = key;
    }
  else
    {
      clt->elements[clt->index] = elem;
    }

  clt->index++;
}

/// Returns an array holding all the elements in the tree
/// in ascending order of their keys (which are not part
/// of the value).
///
/// \param tree pointer to the tree
/// \returns: array of tree_size() elements
T *tree_elements(tree_t *tree)
{
  int size = tree_size(tree);
  T *elements = calloc(size, sizeof(T));
  node_clt clt = { .index = 0, .type = ELEMENTS, .elements = elements };
    
  if (size > 0)
    {
      tree_apply(tree, inorder, collect_nodes, &clt);
    }
  
  return elements;
}

/// Returns an array holding all the keys in the tree
/// in ascending order.
///
/// \param tree pointer to the tree
/// \returns: array of tree_size() keys
K *tree_keys(tree_t *tree)
{
  int size = tree_size(tree);
  K *keys = calloc(size, sizeof(K));
  node_clt clt = { .index = 0, .type = KEYS, .keys = keys };
  
  if (size > 0)
    {
      tree_apply(tree, inorder, collect_nodes, &clt);
    }
  
  return keys;
}

