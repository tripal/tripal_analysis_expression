<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class sep__biological_sample_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Biomaterials Browser';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['sep__biological_sample'];

  // The list of default settings for this formatter.
  public static $default_settings = [
    'setting1' => 'default_value',
  ];

  /**
   * Provides the field's setting form.
   *
   * This function corresponds to the hook_field_formatter_settings_form()
   * function of the Drupal Field API.
   *
   * The settings form appears on the 'Manage Display' page of the content
   * type administration page. This function provides the form that will
   * appear on that page.
   *
   * To add a validate function, please create a static function in the
   * implementing class, and indicate that this function should be used
   * in the form array that is returned by this function.
   *
   * This form will not be displayed if the formatter_settings_summary()
   * function does not return anything.
   *
   * param $field
   *   The field structure being configured.
   * param $instance
   *   The instance structure being configured.
   * param $view_mode
   *   The view mode being configured.
   * param $form
   *   The (entire) configuration form array, which will usually have no use
   *   here.  Typically for reference only.
   * param $form_state
   *   The form state of the (entire) configuration form.
   *
   * @return
   *   A Drupal Form array containing the settings form for this field.
   */
  public function settingsForm($view_mode, $form, &$form_state) {

  }

  /**
   *  Provides the display for a field
   *
   * This function corresponds to the hook_field_formatter_view()
   * function of the Drupal Field API.
   *
   *  This function provides the display for a field when it is viewed on
   *  the web page.  The content returned by the formatter should only include
   *  what is present in the $items[$delta]['values] array. This way, the
   *  contents that are displayed on the page, via webservices and downloaded
   *  into a CSV file will always be identical.  The view need not show all
   *  of the data in the 'values' array.
   *
   * @param $element
   * @param $entity_type
   * @param $entity
   * @param $langcode
   * @param $items
   * @param $display
   *
   * @return void
   *    An element array compatible with that returned by the
   *    hook_field_formatter_view() function.
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $base_table = $entity->chado_table;
    if (empty($items)) {
      return;
    }

    $elements = 1;
    $num_per_page = 10;

    $headers = [
      'Name',
      'Tissue',
      'Treatment',
      'Description',
    ];


    $rows = [];

    foreach ($items as $item) {

      $b_contact = NULL;
      $tissue = NULL;
      $treatment = NULL;
      $description = NULL;

      $biomaterial = $item['value'];
      $b_name = $biomaterial["name"];
      $record = $biomaterial["record_id"];
      $id = $biomaterial["entity_id"];

      if ($id) {
        $b_name = l($b_name, 'bio_data/' . $id);
      }

      //we dont use contact
      //      if (isset($biomaterial["contact_id"])) {
      //        $id = chado_get_record_entity_by_table('contact', $biomaterial["contact_id"]);
      //        $b_contact = $biomaterial["contact_name"];
      //
      //        if ($id) {
      //          $b_contact = l($biomaterial["contact_id"], 'bio_data/' . $id, ['attributes' => ['target' => '_blank']]);
      //        }
      //      }


      $tissue = $biomaterial['tissue'];
      $treatment = $biomaterial['treatment'];

      $description = $biomaterial["description"];
      if (strlen($description) > 300) {
        $description = substr($description, 0, 300);
        $description .= "...";
      }

      $row = [
        $b_name,
        // $b_contact ?: 'Not set',
        $tissue ?: 'Not set',
        $treatment ?: "Not set",
        $description,
      ];

      $rows[] = $row;
    }
    $current_page = pager_default_initialize(count($rows), $num_per_page, 1);
    $chunks = array_chunk($rows, $num_per_page, TRUE);

    $output = theme('table', [
      'header' => $headers,
      'rows' => $chunks[$current_page],
      'attributes' => ['class' => ['table-layout-fixed']],
    ]);

    $output .= theme('pager', [
      'quantity' => 5,
      'element' => $elements,
      'parameters' => ['tripal_pane' => 'gp_Biomaterials'],
    ]);

    $element[] = ['#markup' => $output];


  }

  /**
   * Provides a summary of the formatter settings.
   *
   * This function corresponds to the hook_field_formatter_settings_summary()
   * function of the Drupal Field API.
   *
   * On the 'Manage Display' page of the content type administration page,
   * fields are allowed to provide a settings form.  This settings form can
   * be used to allow the site admin to define how the field should be
   * formatted.  The settings are then available for the formatter()
   * function of this class.  This function provides a text-based description
   * of the settings for the site developer to see.  It appears on the manage
   * display page inline with the field.  A field must always return a
   * value in this function if the settings form gear button is to appear.
   *
   * See the hook_field_formatter_settings_summary() function for more
   * information.
   *
   * @param $field
   * @param $instance
   * @param $view_mode
   *
   * @return string
   *   A string that provides a very brief summary of the field settings
   *   to the user.
   *
   */
  public function settingsSummary($view_mode) {
    return '';
  }
}
