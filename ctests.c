#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "list.c"
#include "tree.c"
#include "db.c"
#include "utils.c"
#include "CUnit/Basic.h"
#include "common.h"



char *name[] = {"Krita", "Kanin", "Dator", "Äpple", "Fanta", "Bok", "Garderob", "Lampa", "Sax", "Pizza"};
char *desc[] = {"Trubbig", "Lätt", "Len", "Askalas", "Snuskig", "Ostig", "Söt", "Vit", "Blöt", "Fluffig"};
int price[] = {3, 5, 7, 11, 13, 1337, 1234567890, 98765431, 69, 42};
char *shelf[] = {"A1", "B2", "C3", "F12", "K37", "H54", "M8", "L82", "S32", "Y66"};
int amount[] = {1, 22, 333, 4444, 5555, 999999999, 1234, 56789, 0, 15};

static FILE* temp_file = NULL;

int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}

int init_suite_tree(void)
{
  return 0;
}

int clean_suite_tree(void)
{
  return 0;
}

int init_suite_list(void)
{
  return 0;
}

int clean_suite_list(void)
{
  return 0;
}

int init_suite_db(void)
{
  if (NULL == (temp_file = fopen("temp.txt", "w+")))
    {
      return -1;
    }
  else
    {
      return 0;
    }
}

int clean_suite_db(void)
{
  if (0 != fclose(temp_file))
    {
      return -1;
    }
  else
    {
      temp_file = NULL;
      return 0;
    }
}

//--------------------------------------------------------------------------------------------
// List

void test_list_new(void)
{
  list_t *list = list_new(NULL, NULL, NULL);
  CU_ASSERT_TRUE( list != NULL );
}


void test_link_new(void)
{
  link_t *link = link_new((elem_t) 1, NULL);
  CU_ASSERT_TRUE( link->value.i == 1);
  CU_ASSERT_FALSE(link->value.i != 1);
}


void test_list_append(void)
{
  list_t *list = list_new(NULL, NULL, NULL);
  for (int i = 0; i < 10 ; i++)
    {
      elem_t tmp = { .p = name[i] };
      list_append(list, tmp);
      CU_ASSERT(list_length(list) == i + 1);
    }
}

list_t *tmp_list_append(void)
{
  list_t *list = list_new(NULL, NULL, NULL);
  for (int i = 0; i < 10 ; i++)
    {
      elem_t tmp = { .p = name[i] };
      list_append(list, tmp);
    }
  return list;
}

void test_list_prepend(void)
{
 
  list_t *list = list_new(NULL, NULL, NULL);
  for (int i = 0; i < 10 ; i++)
    {
      elem_t tmp = { .p = name[i] };
      list_prepend(list, tmp);
      CU_ASSERT_TRUE(list_length(list) == i + 1);
    } 
}

list_t *tmp_list_prepend(void)
{ 
  list_t *list = list_new(NULL, NULL, NULL);
  for (int i = 0; i < 10 ; i++)
    {
      elem_t tmp = { .p = name[i] };
      list_prepend(list, tmp);
    }
  return list; 
}

void test_list_insert(void)
{
  list_t *list = list_new(NULL, NULL, NULL);
  for (int i = 0; i < 10 ; i++)
    {
      elem_t tmp = { .p = name[i] };
      list_insert(list, i, tmp);
      CU_ASSERT_TRUE(list_length(list) == i + 1);
    }
}

list_t *tmp_list_insert(void)
{
  list_t *list = list_new(NULL, NULL, NULL);
  for (int i = 0; i < 10 ; i++)
    {
      elem_t tmp = { .p = strdup(name[i]) };
      list_insert(list, i, tmp);
    }
  return list;
}


void test_list_length(void)
{
  list_t *list = tmp_list_append();
  CU_ASSERT_TRUE(list_length(list) == 10);
  //CU_ASSERT_TRUE(list_length(NULL) == 0);
  CU_ASSERT_FALSE(list_length(list) != 10);
}


void test_list_get(void)
{
  list_t *listappend = tmp_list_append();
  for (int i = 0 ; i < list_length(listappend); i++)
  {
    elem_t tmp1 = { .p = NULL };
    list_get(listappend, i, &tmp1);
    CU_ASSERT_TRUE( strcmp(name[i], tmp1.p ) == 0);
  }

  list_t *listprepend = tmp_list_prepend();
  for (int i = 0 ; i < list_length(listprepend); i++)
  {
    elem_t tmp2 = { .p = NULL };
    list_get(listprepend, i, &tmp2);
    CU_ASSERT_TRUE( strcmp(name[9 - i], tmp2.p ) == 0);
  }

  list_t * listinsert = tmp_list_insert();
  for (int i = 0 ; i < list_length(listinsert); i++)
  {
    elem_t tmp3 = { .p = NULL };
    list_get(listinsert, i, &tmp3);
     CU_ASSERT_TRUE( strcmp(name[i], tmp3.p ) == 0);
  }
}


void test_list_remove(void)
{
  list_t *list = tmp_list_append();
  list_remove(list, 0, false);
  CU_ASSERT_TRUE( list_length(list) == 9);
  list_remove(list, 1, false);
  CU_ASSERT_TRUE( list_length(list) == 8);
  list_remove(list, 2, false);
  CU_ASSERT_TRUE( list_length(list) == 7);
  list_remove(list, 3, false);
  CU_ASSERT_TRUE( list_length(list) == 6);
}



void test_list_delete(void)
{
  list_t *list = tmp_list_append();
  list_delete(list, false);
  CU_ASSERT_TRUE(list != NULL);
}



void test_list_first(void)
{
  list_t *list = tmp_list_append();
  
    elem_t tmp = { .p = NULL };
    list_first(list, &tmp);
    CU_ASSERT_TRUE(name[0] == tmp.p );
    CU_ASSERT_FALSE(name[1] == tmp.p);
    CU_ASSERT_TRUE(NULL != tmp.p);
}


void test_list_last(void)
{
  list_t *list = tmp_list_append();
  
    elem_t tmp = { .p = NULL };
    list_last(list, &tmp);
    CU_ASSERT_TRUE(name[9] == tmp.p );
    CU_ASSERT_FALSE(name[5] == tmp.p);
    CU_ASSERT_TRUE(NULL != tmp.p);
}




//--------------------------------------------------------------------------------------------
// Tree

int tree_cmp(elem_t a, elem_t b)
{
  return strcmp(a.p, b.p);
}

void test_tree_new(void)
{
  tree_t *tree = tree_new(NULL, NULL, NULL, (element_comp_fun) tree_cmp);
  for (int i = 0; i < 10; i++)
    {
      tree_key_t tmpkey = { .p = name[i] };
      elem_t tmpvalue = { .p = desc[i] };
      tree_insert(tree, tmpkey, tmpvalue);
    };
}

tree_t *tmp_tree_new(void)
{
  tree_t *tree = tree_new(NULL, NULL, NULL, (element_comp_fun) tree_cmp);
  for (int i = 0; i < 10; i++)
    {
      tree_key_t tmpkey = { .p = name[i] };
      elem_t tmpvalue = { .p = desc[i] };
      tree_insert(tree, tmpkey, tmpvalue);
    }
  return tree;
}



void test_tree_delete(void)
{
  tree_t *tree = tmp_tree_new();
  tree_delete(tree, false, false);
  CU_ASSERT_TRUE(tree != NULL);
}





void test_tree_size(void)
{
  tree_t *tree = tmp_tree_new();
  CU_ASSERT_TRUE(tree_size(tree) == 10);
  CU_ASSERT_TRUE(tree_size(tree_new(NULL, NULL, NULL, (element_comp_fun) tree_cmp)) == 0);
}


void test_tree_depth(void)
{
  tree_t *tree = tmp_tree_new();
  CU_ASSERT_TRUE(tree_depth(tree) == 4);
  CU_ASSERT_TRUE(tree_depth(tree_new(NULL, NULL, NULL, (element_comp_fun) tree_cmp)) == 0);
}

void test_tree_has_key(void)
{
  tree_t *tree = tmp_tree_new();
  for (int i = 0; i < tree_size(tree); i++)
    {
      tree_key_t tmpkey = { .p = name[i] };
      CU_ASSERT_TRUE(tree_has_key(tree, tmpkey));
    }
}


void test_tree_get(void)
{
  tree_t *tree = tmp_tree_new();
  for (int i = 0; i < tree_size(tree); i++)
    {
      tree_key_t tmpkey = { .p = name[i] };
      elem_t tmpvalue = { .p = NULL };
      tree_get(tree, tmpkey, &tmpvalue);
      CU_ASSERT_TRUE( strcmp(desc[i], tmpvalue.p)  == 0);
    }
  
}

void test_tree_remove(void)
{
  tree_t *tree = tmp_tree_new();
  for (int i = 0; i < 10; i++)
    {
      tree_key_t tmpkey = { .p = name[i] };
      elem_t tmp;
      CU_ASSERT_TRUE(tree_size(tree) == 10-i);
      tree_remove(tree, tmpkey, &tmp);
      CU_ASSERT_TRUE(tree_size(tree) == 9-i);
    }
}

void test_tree_insert(void)
{
  CU_ASSERT_TRUE(tree_size(tmp_tree_new())== 10);
}



//--------------------------------------------------------------------------------------------
// Database


void test_add_goods(void)
{
  tree_t *tree = tree_new(NULL, NULL, NULL, (element_comp_fun) tree_cmp);
  for (int i = 0; i < 10; i++)
    {
      tree_key_t tmp_name = { .p = name[i] };
      char *tmp_desc = desc[i];
      int tmp_price = price[i];
      char *tmp_shelf_name = shelf[i];
      int tmp_shelf_amount = amount[i];
      item_t *item = make_item(tmp_desc, tmp_price);
      add_shelf(item, tmp_shelf_name, tmp_shelf_amount);
      insert_goods(tree, tmp_name, (elem_t){ .p = item });
      CU_ASSERT_TRUE(tree_has_key(tree, (tree_key_t) { .p =name[i] }));
     }
}

tree_t *tmp_add_goods(void)
{
  tree_t *tree = tree_new(NULL, NULL, NULL, (element_comp_fun) tree_cmp);
  for (int i = 0; i < 10; i++)
    {
      tree_key_t tmp_name = { .p = strdup(name[i]) };
      char *tmp_desc = strdup(desc[i]);
      int tmp_price = price[i];
      char *tmp_shelf_name = strdup(shelf[i]);
      int tmp_shelf_amount = amount[i];
      item_t *item = make_item(tmp_desc, tmp_price);
      
      add_shelf(item, tmp_shelf_name, tmp_shelf_amount);
      insert_goods(tree, tmp_name, (elem_t){ .p = item });
      
      CU_ASSERT_TRUE(tree_has_key(tree, (tree_key_t) { .p =name[i] }));
     }
  return tree;
}



void test_remove_goods(void)
{
  tree_t *tree = tmp_add_goods();
  action_t *tmp_action = action_new();
  tree_key_t *keys = tree_keys(tree);
  for (int i = 0; i < 10; i++)
    { 
      elem_t tmp = { .p = NULL };
      tree_get(tree, keys[i], &tmp);
      goods_t goods = { .name = strdup(name[i]), .item = tmp.p };
      remove_from_catalog(tree, goods, tmp_action);
      CU_ASSERT_FALSE(tree_has_key(tree, (tree_key_t) { .p = name[i] }));
    }
}


void test_undo_action(void)
{
  action_t *action = action_new();
  tree_t *tree = tree_new(NULL, NULL, NULL, (element_comp_fun) tree_cmp);

  for (int i = 0; i < 10; ++i)
    {
      tree_key_t key = { .p = strdup(name[i]) };
      char *tmp_desc = strdup(desc[i]);
      int tmp_price = price[i];
      char *tmp_shelf_name = strdup(shelf[i]);
      int tmp_shelf_amount = amount[i];
      item_t *item = make_item(tmp_desc, tmp_price);
      
      add_shelf(item, tmp_shelf_name, tmp_shelf_amount);
      
      tree_insert(tree, key, (elem_t) { .p = item });
    }

  tree_key_t *keys = tree_keys(tree);
  elem_t tmp = { .p = NULL };
  tree_get(tree, keys[0], &tmp);
  goods_t goods = { .name = keys[0].p, .item = tmp.p };

  remove_from_catalog(tree, goods, action);

  undo_action(tree, action);

  CU_ASSERT_TRUE(tree_has_key(tree, keys[0]));

  copy_goods_to_action(action, goods);

  goods.item->description = strdup("desc");

  undo_action(tree, action);
  
  CU_ASSERT_TRUE( strcmp(goods.item->description, "desc") != 0 );
}


int main(void){
  
  CU_pSuite pSuite = NULL;
  CU_pSuite listSuite = NULL;
  CU_pSuite treeSuite = NULL;
  CU_pSuite dbSuite = NULL;
  
  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  /* add a suite to the registry */
  pSuite = CU_add_suite( "db_test_suite", init_suite, clean_suite );
  if ( NULL == pSuite )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }
   
  listSuite = CU_add_suite( "list_test_suite", init_suite, clean_suite );
  if ( NULL == pSuite ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (
      (NULL == CU_add_test(listSuite, "list_new", test_list_new)) ||
      (NULL == CU_add_test(listSuite, "list_append", test_list_append)) ||
      (NULL == CU_add_test(listSuite, "list_prepend", test_list_prepend)) ||
      (NULL == CU_add_test(listSuite, "list_insert", test_list_insert)) ||
      (NULL == CU_add_test(listSuite, "list_length", test_list_length)) ||
      (NULL == CU_add_test(listSuite, "list_get", test_list_get)) ||
      (NULL == CU_add_test(listSuite, "list_remove", test_list_remove)) ||
      (NULL == CU_add_test(listSuite, "list_delete", test_list_delete)) ||
      (NULL == CU_add_test(listSuite, "list_first", test_list_first)) ||
      (NULL == CU_add_test(listSuite, "list_last", test_list_last))
      )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  treeSuite = CU_add_suite( "tree_test_suite", init_suite, clean_suite );
  if ( NULL == pSuite ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (
      (NULL == CU_add_test(treeSuite, "tree_new", test_tree_new)) ||
      (NULL == CU_add_test(treeSuite, "tree_delete", test_tree_delete)) ||
      (NULL == CU_add_test(treeSuite, "tree_size", test_tree_size)) ||
      (NULL == CU_add_test(treeSuite, "tree_depth", test_tree_depth)) ||
      (NULL == CU_add_test(treeSuite, "tree_has_key", test_tree_has_key)) ||
      (NULL == CU_add_test(treeSuite, "tree_get", test_tree_get)) ||
      (NULL == CU_add_test(treeSuite, "tree_remove", test_tree_remove)) ||
      (NULL == CU_add_test(treeSuite, "tree_insert", test_tree_insert))
      )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }
   
  dbSuite = CU_add_suite( "db_test_suite", init_suite, clean_suite );
  if ( NULL == pSuite )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (
      (NULL == CU_add_test(dbSuite, "add_goods", test_add_goods)) ||
      (NULL == CU_add_test(dbSuite, "remove_goods", test_remove_goods)) ||
      (NULL == CU_add_test(dbSuite, "undo_action", test_undo_action))
      )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();

  return CU_get_error();
}


/*
  int main(int argc, char *argv[])
  {

  CU_initialize_registry();
  CU_pSuite suite = NULL;

    CU_add_test(suite, "maxi_fun", test_list_new);
    CU_add_test(suite, "maxi_fun", test_list_new);
    CU_add_test(suite, "maxi_fun", test_list_append);
    CU_add_test(suite, "maxi_fun", test_list_prepend);
    CU_add_test(suite, "maxi_fun", test_list_new);
    CU_add_test(suite, "maxi_fun", test_list_new);
    CU_add_test(suite, "maxi_fun", test_list_new);
    CU_add_test(suite, "maxi_fun", test_list_new);
    CU_add_test(suite, "maxi_fun", test_list_new);
    CU_add_test(suite, "maxi_fun", test_list_new);
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    
  test_list_new();
  test_list_append();
  test_list_prepend();
  test_list_insert();
  test_list_length();
  test_list_get();
  test_list_remove();
  test_list_delete();
  test_list_first();
  test_list_last();


  test_tree_new();
  test_tree_delete();
  test_tree_size();
  test_tree_depth();
  test_tree_has_key();
  test_tree_get();
  test_tree_remove();
  test_tree_insert();


  test_add_goods();
  test_remove_goods();
  return 0;
}
*/
