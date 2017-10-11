

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "tree.h"

typedef struct node node_t;

/// Struct for tree
struct tree {
  node_t *top;
  cmp_t *cmp_f;
  bool balanced;
  int size;
};


/// Types of keys and elements
union element
{
  void *p;
  int   i;
  uint  u;
  float f;
};

/// Node struct
struct node {
  element_t key;
  element_t element;
  node_t *left;
  node_t *right;
};

/// Enum for simplifying result of key compares
enum key_compare { KEYS_MATCH, MOVE_RIGHT, MOVE_LEFT };

/// Struct for keeping track of collection of nodes
typedef struct {
  int index;
  enum { KEYS, ELEMENTS } type;
  union {
    element_t *keys;
    element_t *elements;
  };
} node_clt;

/// Creates a new tree
///
/// \param cmp compare function for keys
/// \returns: empty tree
tree_t *tree_new(cmp_t *cmp)
{
  tree_t *new = calloc(1, sizeof(tree_t));

  if (new)
    {
      new->cmp_f = cmp;
      new->balanced = true;
    }

  return new;
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
  free(node);
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
  if (node == NULL)
    {
      return 0;
    }
  else if (node->left == NULL && node->right == NULL)
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
/// \param tree associated tree with compare function
/// \param node key of node
/// \param key key to compare with
/// \returns direction to move in
enum key_compare compare_keys(tree_t *tree, element_t node_key, element_t cmp_key)
{
  int a = (*tree->cmp_f)(node_key, cmp_key);

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
node_t **search_tree(tree_t *tree, element_t key)
{
  node_t **node = &(tree->top);
  
  while (*node != NULL)
    {
      switch(compare_keys(tree, (*node)->key, key))
	{
	case KEYS_MATCH:
	  return node;
	  break;

	case MOVE_LEFT:
	  node = &(*node)->left;
	  break;

	case MOVE_RIGHT:
	  node = &(*node)->right;
	  break;

	default:
	  // Something's broken
	  assert(false);
	  break;
	}
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

/// Determine if a tree is balanced
///
/// \param tree tree to check
/// \return true if tree is balanced, otherwise false
bool check_balanced(tree_t *tree)
{
  int left_depth = count_depth(tree->top->left);
  int right_depth = count_depth(tree->top->right);
  int balance_factor = abs(left_depth - right_depth);
  return balance_factor < 2;
}

/// Check if a tree has been flagged as balanced
///
/// \param tree tree to check
/// \return true if tree is balanced, otherwise false
bool is_balanced(tree_t *tree)
{
  assert(tree);
  return tree->balanced;
}
 
/// Rotates nodes according to AVL right rotation
///
/// \param A top node
/// \returns new top node
node_t *avl_rotate_left(node_t *A)
{
  node_t *B = A->right;
  node_t *B_left = B->left;

  B->left = A;
  A->right = B_left;
  
  return B;
}

/// Rotates nodes according to AVL right rotation
///
/// \param C top node
/// \returns new top node
node_t *avl_rotate_right(node_t *C)
{
  node_t *B = C->left;
  node_t *B_right = B->right;

  B->right = C;
  C->left = B_right;
  
  return B;
}

/// Rotates nodes according to AVL left-right rotation
///
/// \param C top node
/// \returns new top node
node_t *avl_rotate_left_right(node_t *C)
{
  node_t *B = C->left;
  node_t *A = B->right;

  node_t *B_left = B->left;

  B->left = A;
  C->left = B;
  A->right = B_left;
  
  return avl_rotate_right(B);
}

/// Rotates nodes according to AVL right-left rotation
///
/// \param C top node
/// \returns new top node
node_t *avl_rotate_right_left(node_t *A)
{
  node_t *C = A->right;
  node_t *B = C->left;

  node_t *B_right = B->right;

  B->right = C;
  A->right = B;
  C->left = B_right;
  
  return avl_rotate_left(B);
}

/// Balances subtree
///
/// \param tree tree that is being balanced
/// \param node top node of current subtree
/// \returns top node in balanced subtree
node_t *balance_subtree(tree_t *tree, node_t *node)
{ 
  // Balance subtrees
  if (node->left) node->left = balance_subtree(tree, node->left);
  if (node->right) node->right = balance_subtree(tree, node->right);

  // No need to continue balancing if tree is balanced
  if (is_balanced(tree)) return node;

  int left_depth = count_depth(node->left);
  int right_depth = count_depth(node->right);
  int balance_factor = left_depth - right_depth;

  // Child node to return
  node_t *child = node;
  
  if (balance_factor < -1)
    {
      // Too many  nodes in right subtree
      if (count_depth(node->right->left) > count_depth(node->right->right))
	{
	  child = avl_rotate_right_left(node);
	}
      else 
	{
	  child = avl_rotate_left(node);
	}
    }
  else if (balance_factor > 1)
    {
      // Too many nodes in left subtree
      if (count_depth(node->left->right) > count_depth(node->left->left))
	{
	  child = avl_rotate_left_right(node);
	}
      else
	{
	  child = avl_rotate_right(node);
	}
    }

  tree->balanced = check_balanced(tree);
  
  return child;
}

/// Balances a tree.
///
/// \param tree tree to be balanced
/// \returns if balancing was succesful
bool balance_tree(tree_t *tree)
{
  node_t *top = tree->top;
  
  if (check_balanced(tree))
    {
      // No need to rebalance
      return true;
    }
  else
    {
      tree->balanced = false;
      tree->top = balance_subtree(tree, top);
      return true;
    }

  return false;
}

/// Insert element into the tree. Returns false if the key is already used.
/// Balances tree after insert.
///
/// \param tree pointer to the tree
/// \param key the key of element to be appended
/// \param elem the element 
/// \returns: true if successful, else false
bool tree_insert(tree_t *tree, element_t key, element_t elem)
{
  if (tree == NULL) return false;
  
  node_t *new = calloc(1, sizeof(node_t));
  new->key = key;
  new->element = elem;

  node_t **leaf = search_tree(tree, key);

  if (*leaf == NULL)
    {
      *leaf = new;
      ++tree->size;
      balance_tree(tree);
      assert(check_balanced(tree));
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
bool tree_has_key(tree_t *tree, element_t key)
{
  assert(tree);
  return *(search_tree(tree, key)) != NULL;
}

/// Returns the element for a given key in tree.
/// (The implementation may assume that the key exists.)
/// 
/// \param tree pointer to the tree
/// \param key the key of elem to be removed
/// \returns: true if key is a key in tree
element_t tree_get(tree_t *tree, element_t key)
{
  assert(tree);
  return (*search_tree(tree, key))->element;
}

/// Adds key or elem to node_clt, tree_action2 for tree_elements
/// and tree_keys
///
/// \param key key of node
/// \param elem element of node
/// \param data node_clt to add to
void collect_nodes(element_t key, element_t elem, void *data)
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
element_t *tree_elements(tree_t *tree)
{
  assert(tree);
  int size = tree_size(tree);
  element_t *elements = calloc(size, sizeof(element_t));
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
element_t *tree_keys(tree_t *tree)
{
  assert(tree);
  int size = tree_size(tree);
  element_t *keys = calloc(size, sizeof(element_t));
  node_clt clt = { .index = 0, .type = KEYS, .keys = keys };
  
  if (size > 0)
    {
      tree_apply(tree, inorder, collect_nodes, &clt);
    }
  
  return keys;
}

