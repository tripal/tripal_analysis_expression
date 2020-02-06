<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__expression_features_formatter extends ChadoFieldFormatter
{

  // The default label for this field.
  public static $default_label = 'Expression Features';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('local__expression_features');

  // The list of default settings for this formatter.
  public static $default_settings = array(
    'setting1' => 'default_value',
  );

  /**
   * @see ChadoFieldFormatter::settingsForm()
   *
   **/

  public function settingsForm($view_mode, $form, &$form_state)
  {

  }

  /**
   * @see ChadoFieldFormatter::View()
   *
   **/

  public function view(&$element, $entity_type, $entity, $langcode, $items, $display)
  {
    $rows = $items[0]['value'];
    if ($rows == []) {
      $element[0] = ['#markup' => 'No feature found'];
      return $element;
    }

    $element[0] =
      [
        "#type" => "markup",
        "#markup" => theme('table',
          [
            "header" => $items[0]['header'],
            "rows" => $rows,
          ])
      ];
  }

  /**
   * @see ChadoFieldFormatter::settingsSummary()
   *
   **/

  public function settingsSummary($view_mode)
  {
    return '';
  }

}
