<?php
/**
 * @file
 * Contains all field specific code outside the classes.
 */

/**
 * @implements hook_bundle_fields_info_alter()
 *
 * @param $fields
 * @param $bundle
 * @param $term
 */
function tripal_protocol_bundle_fields_info_alter(&$fields, $bundle, $term) {
  if (isset($bundle->data_table) AND ($bundle->data_table == 'protocol')) {
    $fields['schema__url']['type'] = 'text';
    $fields['schema__name']['type'] = 'text';
  }

  if (isset($bundle->data_table) AND ($bundle->data_table == 'arraydesign')) {
    $fields['local__manufacturer_id']['type'] = 'chado_linker__contact';
    $fields['local__manufacturer_id']['widget']['settings']['chado_table'] = 'contact';


  }
}

/**
 * @implements hook_bundle_instances_info_alter()
 *
 * @param $instances
 * @param $bundle
 * @param $term
 */
function tripal_protocol_bundle_instances_info_alter(&$instances, $bundle, $term) {
  if (isset($bundle->data_table) AND ($bundle->data_table == 'protocol')) {
    $text_fields = [
      'local__protocol_description',
      'local__software_description',
      'local__hardware_description',
    ];

    foreach ($text_fields as $field_name) {
      $parts = explode('__', $field_name);
      $label = implode(' ', explode('_', $parts[1]));
      $label = ucwords($label);
      $instances[$field_name]['label'] = $label;
    }

    $instances['schema__name']['label'] = 'Name';
    $instances['schema__name']['required'] = TRUE;
    $instances['schema__name']['widget']['type'] = 'text_textfield';
    $instances['schema__name']['settings']['text_processing'] = '0';
    $instances['schema__name']['weight'] = 0;

    $instances['schema__url']['label'] = 'Link';
    $instances['schema__url']['widget']['type'] = 'text_textfield';
    $instances['schema__url']['settings']['text_processing'] = '0';
    $instances['schema__url']['required'] = TRUE;
    $instances['schema__url']['weight'] = 1;
  }

  // Not gonna change manufacturer id to contacts type until the contacts field
  // widget is capable of recognizing the correct column name (manufacturer_id
  // rather than contact_id).
  if (isset($bundle->data_table) AND ($bundle->data_table == 'arraydesign')) {

    $instances['local__manufacturer_id']['label'] = 'Manufacturer';
    $instances['local__manufacturer_id']['widget']['type'] = 'chado_linker__contact_widget';
    $instances['local__manufacturer_id']['widget']['settings']['chado_table'] = 'contact';
    $instances['local__manufacturer_id']['settings']['chado_table'] = 'contact';
  }
}

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
function tripal_protocol_bundle_fields_info($entity_type, $bundle) {
  $fields = [];

  // Because we are expecting data housed in Chado we can use the 'data_table'
  // property of the bundle to determine if this bundle is really the one
  // we want the field to be associated with.
  if (isset($bundle->data_table) AND ($bundle->data_table == 'protocol')) {
    // First add my term.
    tripal_insert_cvterm([
      'id' => 'local:protocol_type',
      'name' => 'protocol_type',
      'cv_name' => 'local',
      'definition' => 'Protocol type',
    ]);

    // Then describe the field defined by that term.
    $field_name = 'local__protocol_type';
    $field_type = 'local__protocol_type';
    $fields[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
    ];
  }

  if (isset($bundle->data_table) AND ($bundle->data_table == 'arraydesign')) {
    // First add my term.
    tripal_insert_cvterm(array(
      'id' => 'OBI:0000835',
      'name' => 'manufacturer',
      'cv_name' => 'OBI',
      'definition' => 'A person or organization that has a manufacturer role',
    ));

    // Then describe the field defined by that term.
    $field_name = 'obi__manufacturer';
    $field_type = 'obi__manufacturer';
    $fields[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
    );

    //microarray type
    tripal_insert_cvterm(array(
      'id' => 'OBI:0000052',
      'name' => 'microarray platform',
      'cv_name' => 'OBI',
      'definition' => 'A microarray platform is a platform that contains the instruments, software and reagents needed to perform a microarray protocol',
    ));

    // Then describe the field defined by that term.
    $field_name = 'obi__microarray_platform';
    $field_type = 'obi__microarray_platform';
    $fields[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
    );

    //Substrate
    tripal_insert_cvterm(array(
      'id' => 'EFO:0005061',
      'name' => 'substrate',
      'cv_name' => 'EFO',
      'definition' => 'A substrate is the surface on which material, such as an organism, is grown or attached.',
    ));

    // Then describe the field defined by that term.
    $field_name = 'efo__substrate';
    $field_type = 'efo__substrate';
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
function tripal_protocol_bundle_instances_info($entity_type, $bundle) {
  $instances = [];

  if (isset($bundle->data_table) AND ($bundle->data_table == 'protocol')) {
    $field_name = 'local__protocol_type';
    $field_type = 'local__protocol_type';
    $instances[$field_name] = [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Protocol Type',
      'description' => 'Protocol type',
      'required' => TRUE,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => $bundle->data_table,
        'chado_column' => 'type_id',
        'base_table' => $bundle->data_table,
      ],
      'widget' => [
        'type' => 'local__protocol_type_widget',
        'settings' => [],
      ],
      'display' => [
        'default' => [
          'label' => 'hidden',
          'type' => 'local__protocol_type_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  if (isset($bundle->data_table) AND ($bundle->data_table == 'arraydesign')) {
    $field_name = 'obi__manufacturer';
    $field_type = 'obi__manufacturer';
    $instances[$field_name] =  array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'manufacturer',
      'description' => 'Manufacturer of an entity ie an array',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => TRUE,
        'chado_table' => $bundle->data_table,
        'chado_column' => 'manufacturer_id',
        'base_table' => $bundle->data_table,
      ),
      'widget' => array(
        'type' => 'obi__manufacturer_widget',
        'settings' => array(),
      ),
      'display' => array(
        'default' => array(
          'label' => 'hidden',
          'type' => 'obi__manufacturer_formatter',
          'settings' => array(),
        ),
      ),
    );

    // microarray design.

      $field_name = 'obi__microarray_platform';
      $field_type = 'obi__microarray_platform';
      $instances[$field_name] =  array(
        'field_name' => $field_name,
        'entity_type' => $entity_type,
        'bundle' => $bundle->name,
        'label' => 'Microarray platform',
        'description' => 'A microarray platform is a platform that contains the instruments, software and reagents needed to perform a microarray protocol',
        'required' => FALSE,
        'settings' => array(
          'auto_attach' => FALSE,
          'chado_table' => $bundle->data_table,
          'chado_column' => 'platformtype_id',
          'base_table' => $bundle->data_table,
        ),
        'widget' => array(
          'type' => 'obi__microarray_platform_widget',
          'settings' => array(),
        ),
        'display' => array(
          'default' => array(
            'label' => 'hidden',
            'type' => 'obi__microarray_platform_formatter',
            'settings' => array(),
          ),
        ),
      );

      //substrate
    $field_name = 'efo__substrate';
    $field_type = 'efo__substrate';
    $instances[$field_name] =  array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Microarray substrate',
      'description' => 'The material used for the substrate of a microarray (ie glass, nylon)',
      'required' => FALSE,
      'settings' => array(
        'auto_attach' => FALSE,
        'chado_table' => $bundle->data_table,
        'chado_column' => 'substratetype_id',
        'base_table' => $bundle->data_table,
      ),
      'widget' => array(
        'type' => 'efo__substrate_widget',
        'settings' => array(),
      ),
      'display' => array(
        'default' => array(
          'label' => 'hidden',
          'type' => 'efo__substrate_formatter',
          'settings' => array(),
        ),
      ),
    );



  }

  return $instances;
}
