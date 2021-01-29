<?php
/**
 * @file
 * Contains all field specific code outside the classes.
 */

/**
 * Implements hook_bundle_fields_info().
 *
 * This hook tells Drupal/Tripal about your new field type. Make sure you've
 * created the field (handles basic storage of your data), widget (provides
 * user UI to set data), and formatter (describes display of data on Entity
 * Page) classes. These should be located in the following directory: [your
 * module]/includes/TripalFields/[classname].inc with one file per class. Your
 * field name should be [cv name]__[cvterm name] and the classes should be
 * named [field name], [field_name]_widget, [field name]_formatter for the
 * field, widget and formatter respectively. MAKE SURE YOU'VE CLEARED THE CACHE
 * SINCE ADDING THESE FILES so Tripal magic can find them or the following will
 * fail.
 *
 * @param $entity_type
 *   This should be 'TripalEntity' for all Tripal Content.
 * @param $bundle
 *   This object describes the Type of Tripal Entity (e.g. Organism or Gene)
 *   this hook is being called for. However, since this hook creates field
 *   types (by definition not tied to a specific Tripal Content Type (bundle))
 *   and since a field type will only be created if it doesn't already exist,
 *   this parameter doesn't actually matter. NOTE: If you do need to determine
 *   the bundle in this hook, we suggest inspecting the data_table since the
 *   label can be changed by site administrators.
 *
 * @return
 *   An array of field definitions. Each field in this array will be created if
 *   it doesn't already exist. To trigger create of fields when developing call
 *   tripal_refresh_bundle_fields() for the specific bundle.
 */
function tripal_analysis_expression_bundle_fields_info($entity_type, $bundle)
{
  $fields = [];

  // Because we are expecting data housed in Chado we can use the 'data_table'
  // property of the bundle to determine if this bundle is really the one
  // we want the field to be associated with.
  if (isset($bundle->data_table) AND ($bundle->data_table == 'feature')) {

    $field_name = 'data__gene_expression_data';
    $field_type = 'data__gene_expression_data';
    $fields[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => false,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];

    tripal_insert_cvterm([
      'id' => 'local:expression_details',
      'name' => 'expression_details',
      'cv_name' => 'local',
      'definition' => 'Evidence-based annotation with p-values',
    ]);

    $field_name = 'local__expression_details';
    $field_type = 'local__expression_details';
    $fields[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => false,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  if (isset($bundle->data_table) AND ($bundle->data_table == 'analysis')) {
    $field_name = 'data__gene_expression_data';
    $field_type = 'data__gene_expression_data';
    $fields[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => false,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }


  if (isset($bundle->data_table) AND ($bundle->data_table == 'analysis') AND ($bundle->term->name == 'Gene expression profiling')) {
    // First add my term.
    tripal_insert_cvterm(array(
      'id' => 'local:expression_features',
      'name' => 'expression_features',
      'cv_name' => 'local',
      'definition' => 'Table of expression features.',
    ));

    // Then describe the field defined by that term.
    $field_name = 'local__expression_features';
    $field_type = 'local__expression_features';
    $fields[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
    );
  }

  if (isset($bundle->data_table) AND ($bundle->data_table == 'analysis')) {
    // First add my term.
    tripal_insert_cvterm(array(
      'id' => 'local:pvalue_keyword',
      'name' => 'expression_search',
      'cv_name' => 'local',
      'definition' => 'List of p-value keywords used in annotating individual features.',
    ));

    // Then describe the field defined by that term.
    $field_name = 'local__expression_search';
    $field_type = 'local__expression_search';
    $fields[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
    );
  }
  return $fields;
}

/**
 * Implements hook_bundle_instances_info().
 *
 * This hook tells Drupal/Tripal to create a field instance of a given field
 * type on a specific Tripal Content type (otherwise known as the bundle). Make
 * sure to implement hook_bundle_create_fields() to create your field type
 * before trying to create an instance of that field.
 *
 * @param $entity_type
 *   This should be 'TripalEntity' for all Tripal Content.
 * @param $bundle
 *   This object describes the Type of Tripal Entity (e.g. Organism or Gene)
 *   the field instances are being created for. Thus this hook is called once
 *   per Tripal Content Type on your site. The name of the bundle is the
 *   machine name of the type (e.g. bio_data_1) and the label of the bundle
 *   (e.g. Organism) is what you see in the interface. Since the label can be
 *   changed by site admin, we suggest checking the data_table to determine if
 *   this is the entity you want to add field instances to.
 *
 * @return
 *   An array of field instance definitions. This is where you can define the
 *   defaults for any settings you use in your field. Each entry in this array
 *   will be used to create an instance of an already existing field.
 */
function tripal_analysis_expression_bundle_instances_info($entity_type, $bundle)
{
  $instances = [];

  // FEATURE.
  //===============
  if (isset($bundle->data_table) AND ($bundle->data_table == 'feature')) {
    $field_name = 'data__gene_expression_data';
    $instances[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Expression',
      'description' => 'Visualize expression data',
      'required' => false,
      'settings' => [
        'term_vocabulary' => 'data',
        'term_name' => 'gene_expression_data',
        'term_accession' => '2603',
        'auto_attach' => true,
        // 'chado_table' => $bundle->data_table,
        // 'chado_column' => 'feature_id',
        // 'base_table' => $bundle->data_table,
      ],
      'widget' => [
        'type' => 'data__gene_expression_data_widget',
        'settings' => [],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'data__gene_expression_data_formatter',
          'settings' => [],
        ],
      ],
    ];

    $field_name = 'local__expression_details';
    $instances[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Pvalue Annotation',
      'description' => 'Evidence-based annotation with p-values',
      'required' => FALSE,
      'settings' => [
        'term_vocabulary' => 'local',
        'term_name' => 'expression_details',
        'term_accession' => 'expression_details',
        'auto_attach' => FALSE,
        // 'chado_table' => $bundle->data_table,
        // 'chado_column' => 'organism_id',
        // 'base_table' => $bundle->data_table,
      ],
      'widget' => [
        'type' => 'local__expression_details_widget',
        'settings' => [],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'local__expression_details_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  if (isset($bundle->data_table) AND ($bundle->data_table == 'analysis')) {
    $field_name = 'data__gene_expression_data';
    $instances[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Expression Data',
      'description' => 'Access expression data',
      'required' => false,
      'settings' => [
        'term_vocabulary' => 'data',
        'term_name' => 'gene_expression_data',
        'term_accession' => '2603',
        'auto_attach' => true,
        // 'chado_table' => $bundle->data_table,
        // 'chado_column' => 'analysis_id',
        // 'base_table' => $bundle->data_table,
      ],
      'widget' => [
        'type' => 'data__gene_expression_data_widget',
        'settings' => [],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'data__gene_expression_data_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  if (isset($bundle->data_table) AND ($bundle->data_table == 'analysis') AND ($bundle->term->name == 'Gene expression profiling')) {
    $field_name = 'local__expression_features';
    $field_type = 'local__expression_features';
    $instances[$field_name] = array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Expression Features',
      'description' => 'Table of expression features.',
      'required' => FALSE,
      'settings' => array(
        'term_vocabulary' => 'local',
        'term_name' => 'expression_features',
        'term_accession' => 'expression_features',
        'auto_attach' => FALSE,
        // 'chado_table' => $bundle->data_table,
        // 'chado_column' => 'organism_id',
        // 'base_table' => $bundle->data_table,
      ),
      'widget' => array(
        'type' => 'local__expression_features_widget',
        'settings' => array(),
      ),
      'display' => array(
        'default' => array(
          'label' => 'hidden',
          'type' => 'local__expression_features_formatter',
          'settings' => array(),
        ),
      ),
    );

  }

  if (isset($bundle->data_table) AND ($bundle->data_table == 'analysis')) {
    $field_name = 'local__expression_search';
    $field_type = 'local__expression_search';
    $instances[$field_name] = array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Analysis Keywords',
      'description' => 'List of p-value keywords used in annotating individual features.',
      'required' => FALSE,
      'settings' => array(
        'term_vocabulary' => 'local',
        'term_name' => 'expression_search',
        'term_accession' => 'pvalue_keyword',
        'auto_attach' => TRUE,
        // 'chado_table' => $bundle->data_table,
        // 'chado_column' => 'organism_id',
        // 'base_table' => $bundle->data_table,
      ),
      'widget' => array(
        'type' => 'local__expression_search_widget',
        'settings' => array(),
      ),
      'display' => array(
        'default' => array(
          'label' => 'hidden',
          'type' => 'local__expression_search_formatter',
          'settings' => array(),
        ),
      ),
    );

  }
  return $instances;
}
