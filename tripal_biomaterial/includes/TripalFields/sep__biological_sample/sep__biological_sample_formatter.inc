<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class sep__biological_sample_formatter extends ChadoFieldFormatter{

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
   * @param $data_items
   * @param $display
   *
   * @return void
   *    An element array compatible with that returned by the
   *    hook_field_formatter_view() function.
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $base_table = $entity->chado_table;

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    $name_term = chado_get_semweb_term('biomaterial', 'name');
    $description_term = chado_get_semweb_term('biomaterial', 'description');
    // Use fixed terms for these values which derive from the biomaterialprop table, 
    // type_id + value columns. This module's installer loads biomaterial_property cv 
    // and links it to tripal db. tripal_eutils module uses ncbi_properties cv linked 
    // to ncbi_properties db. Static term here allows this field to work with both.
    $tissue_term = 'tripal:tissue';
    $treatment_term = 'tripal:treatment';

    // Generate the table content.
    $headers = [
      'Name',
      'Tissue',
      'Treatment',
      'Description',
    ];
    $rows = [];
    foreach ($items as $delta => $item) {
      if (empty($item['value'])) {
        continue;
      }
      $name = $item['value'][$name_term];
      if (array_key_exists('entity', $item['value'])) {
        list($entity_type, $entity_id) = explode(':', $item['value']['entity']);
        $name = l(strip_tags($name), 'bio_data/'.$entity_id);
      }
      $rows[] = [
        $name,
        $item['value'][$tissue_term] ?: 'Not set',
        $item['value'][$treatment_term] ?: 'Not set',
        $item['value'][$description_term] ?: '',
      ];
    }

    // The table is not diplayed if there is no content.
    if (!count($rows)) {
      $element[] = [
        '#type' => 'markup',
        '#markup' => 'There are no linked biosamples.'
      ];
      return;
    }

    // Build the pager
    $items_per_page = array_key_exists('items_per_page', $this->instance['settings']) ? $this->instance['settings']['items_per_page'] : 10;
    $total_records = count($rows);
    $total_pages = (int) ($total_records / $items_per_page) + 1;
    $pelement = 0;
    $current_page = pager_default_initialize($total_records, $items_per_page, $pelement);
    $pager = theme('pager', [
      'tags' => [],
      'element' => $pelement,
      'parameters' => [],
      'quantity' => $total_pages,
    ]);
    $pager = $this->ajaxifyPager($pager, $entity);
    $page_items = array_chunk($rows, $items_per_page);

    $caption = '';
    if ($total_records == 1) {
      $caption = 'There is ' . count($rows) . ' biosample.';
    }
    else {
      $caption = 'There are ' . count($rows) . ' biosamples.';
    }
    $content = theme_table([
      'header' => $headers,
      'rows' => count($rows) > 0 ? $page_items[$current_page] : [],
      'attributes' => [
        'class' => 'tripal-data-table',
      ],
      'sticky' => FALSE,
      'caption' => $caption,
      'colgroups' => [],
      'empty' => 'There are no linked biomaterials.',
    ]);

    $element[] = [
      '#type' => 'markup',
      '#markup' => $content . $pager,
    ];
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
