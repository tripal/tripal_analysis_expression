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
    $field = 'sep__biological_sample';
    $variables = $entity->{$field}['und'][0];
    $base_table = $entity->chado_table;
    if (!empty($variables['biomaterials'])) {
      $elements = 1;
      $num_per_page = 10;

      $biomaterials = $variables['biomaterials'];

      $output = '<div class="tripal_organism-data-block-desc tripal-data-block-desc">The following browser provides a list of biomaterials associated with this ' . $base_table . '.</div>';

      $headers = ['Biomaterial Name', 'Biomaterial Provider'];

      if ($variables['show_organism']) {
        $headers[] = 'Organism';
      }

      $rows = [];

      foreach ($biomaterials as $biomaterial) {
        $b_name = $biomaterial->name;
        $b_contact = '';
        $id = chado_get_record_entity_by_table('biomaterial', $biomaterial->biomaterial_id);

        if ($id) {
          $b_name = l($b_name, 'bio_data/' . $id);
        }

        if ($biomaterial->contact_id) {
          $id = chado_get_record_entity_by_table('contact', $biomaterial->contact_id);
          $b_contact = $biomaterial->contact_name;

          if ($id) {
            $b_contact = l($biomaterial->contact_name, 'bio_data/' . $id, ['attributes' => ['target' => '_blank']]);
          }
        }

        $row = [
          $b_name,
          $b_contact ?: 'Unknown',
        ];

        if ($variables['show_organism']) {
          $id = chado_get_record_entity_by_table('organism', $biomaterial->organism_id);
          $organism_name = "{$biomaterial->common_name} ({$biomaterial->genus} {$biomaterial->species})";

          if ($id) {
            $organism_name = l($organism_name, "bio_data/{$id}", ['attributes' => ['target' => '_blank']]);
          }

          $row[] = $organism_name;
        }

        $rows[] = $row;
      }

      $current_page = pager_default_initialize(count($rows), $num_per_page, 1);
      $chunks = array_chunk($rows, $num_per_page, TRUE);

      $output .= theme('table', [
        'header' => $headers,
        'rows' => $chunks[$current_page],
      ]);

      $output .= theme('pager', [
        'quantity' => 5,
        'element' => $elements,
        'parameters' => ['block' => 'biomaterial_browser'],
      ]);
    }
    else {
      $output = '';
    }

    $element[] = [
      '#markup' => $output,
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