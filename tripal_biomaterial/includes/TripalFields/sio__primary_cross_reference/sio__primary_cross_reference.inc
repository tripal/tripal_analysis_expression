<?php
/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class sio__primary_cross_reference extends ChadoField {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'primary cross reference';

  // The default description for this field.
  public static $default_description = 'The primary cross reference is a database cross reference in which one entity is equivalent to the other and it serves as the primary reference.';

  // The default widget for this field.
  public static $default_widget = 'sio__primary_cross_reference_widget';

  // The default formatter for this field.
  public static $default_formatter = 'sio__primary_cross_reference_formatter';

  // The module that manages this field.
  public static $module = 'tripal_biomaterial';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = [
    'storage' => 'field_chado_storage',
    // It is expected that all fields set a 'value' in the load() function.
    // In many cases, the value may be an associative array of key/value pairs.
    // In order for Tripal to provide context for all data, the keys should
    // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
    // function that are supported by the query() function should be
    // listed here.
    'searchable_keys' => [],
  ];

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The DATABASE name, as it appears in chado.db.  This also builds the link-out url.  In most cases this will simply be the CV name.  In some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'SIO',
    // The name of the term.
    'term_name' => 'primary_cross_reference',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '001172',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asynchronously.
    'auto_attach' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => 'dbxref',
    // The column of the table in Chado where the value of the field comes from.
    'chado_column' => 'dbxref_id',
    // The base table.
    'base_table' => 'biomaterial',
  ];

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;

  // A boolean specifying that the field will not contain any data. This
  // should exclude the field from web services or downloads.  An example
  // could be a quick search field that appears on the page that redirects
  // the user but otherwise provides no data.
  public static $no_data = FALSE;

  /**
   * Loads the field values from the underlying data store.
   *
   * @param $entity
   *
   * @return
   *   An array of the following format:
   *     $entity->{$field_name}['und'][0]['value'] = $value;
   *   where:
   *     - $entity is the entity object to which this field is attached.
   *     - $field_name is the name of this field
   *     - 'und' is the language code (in this case 'und' == undefined)
   *     - 0 is the cardinality.  Increment by 1 when more than one item is
   *       available.
   *     - 'value' is the key indicating the value of this field. It should
   *       always be set.  The value of the 'value' key will be the contents
   *       used for web services and for downloadable content.  The value
   *       should be of the follow format types: 1) A single value (text,
   *       numeric, etc.) 2) An array of key value pair. 3) If multiple entries
   *       then cardinality should incremented and format types 1 and 2 should
   *       be used for each item.
   *   The array may contain as many other keys at the same level as 'value'
   *   but those keys are for internal field use and are not considered the
   *   value of the field.
   *
   *
   */
  public function load($entity) {
    parent::load($entity);
    //
    //    $record = $entity->chado_record;
    //
    //    $field_name = $this->field['field_name'];
    //    $field_type = $this->field['type'];
    //    $field_table = $this->instance['settings']['chado_table'];
    //    $field_column = $this->instance['settings']['chado_column'];
    //    $base_table = $record->tablename;
    //
    //    $schema = chado_get_schema($field_table);
    //    $pkey = $schema['primary key'][0];
    //    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    //    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];
    //
    //    $dbname_term = tripal_get_chado_semweb_term('db', 'name');
    //    $accession_term = tripal_get_chado_semweb_term('dbxref', 'accession');
    //    $dburl_term = tripal_get_chado_semweb_term('db', 'url');
    //
    //    // Set some defaults for the empty record.
    //    $entity->{$field_name}['und'][0] = [
    //      'value' => '',
    //      'chado-' . $field_table . '__' . $pkey => '',
    //      'chado-' . $field_table . '__' . $fkey_lcolumn => $record->$fkey_rcolumn,
    //      'chado-' . $field_table . '__dbxref_id' => '',
    //      'db_id' => '',
    //      'accession' => '',
    //    ];
    //
    //    $linker_table = $base_table . '_dbxref';
    //    $options = ['return_array' => 1];
    //    dpm($record);
    //    dpm(chado_expand_var($record, 'table', $linker_table, $options));
    //    $record = chado_expand_var($record, 'table', $linker_table, $options);
    //
    //
    //    $i = 0;
    //
    //    if (count($record->$linker_table) > 0) {
    //      foreach ($record->$linker_table as $index => $linker) {
    //        $dbxref = $linker->dbxref_id;
    //
    //        // Ignore the GFF_source database. This is a weird thing required by
    //        // GBrowse and is added by the GFF loader. We don't want to show it.
    //        if ($dbxref->db_id->name == 'GFF_source') {
    //          continue;
    //        }
    //
    //        $URL = tripal_get_dbxref_url($dbxref);
    //        $entity->{$field_name}['und'][$i] = [
    //          'value' => [
    //            $dbname_term => $dbxref->db_id->name,
    //            $accession_term => $dbxref->accession,
    //            $dburl_term => $URL,
    //          ],
    //          'chado-' . $field_table . '__' . $pkey => $linker->$pkey,
    //          'chado-' . $field_table . '__' . $fkey_lcolumn => $linker->$fkey_lcolumn->$fkey_lcolumn,
    //          'chado-' . $field_table . '__dbxref_id' => $dbxref->dbxref_id,
    //          'db_id' => $dbxref->db_id->db_id,
    //          'accession' => $dbxref->accession,
    //
    //        ];
    //        $i++;
    //      }
    //    }
    //  }
  }
}