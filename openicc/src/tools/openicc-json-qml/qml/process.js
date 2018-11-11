/** @file process.js
 *
 *  OpenICC JSON QML is a graphical renderer of UI files.
 *
 *  @par Copyright:
 *            2018 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2018/02/26
 *
 *  helpers and data in JavaScript
 */

/** This function assumes a data struct to extract a translation.
 *
 *  param[in]    item       { "base": "non translated",
 *                            "base.cs_CZ": "first language",
 *                            "base.de_CH": "second language",
 *                            "base.fr_FR": "thierd language",
 *                            "other": "more info" }
 *  param[in]    base       base name to search translations for
 *  param[in]    loc        desired locale string in language_country form;
 *                          e.g. "de_AT"
 *  return                  nearest match for base.loc; e.g.:
 *                          base.de_AT or base.de_XX or base
 */
function getTranslatedItem( item, base, loc )
{
    var language = "";
    if(loc.length)
        language = loc.substr(0,2);

    var text = "";
    var id = base + "." + loc;

    if(typeof item[id] !== "undefined")
        text = item[id];
    else
    for( var i in item )
    {
        var n = i.indexOf('.');
        var keyn = i.substr(0,n);
        var match = keyn.match(base)
        var type = typeof match;
        if(type !== "object" ||
           match === null ||
           (type === "object" && match[0] !== base) ||
           n === -1)
            continue;
        var prop = i.substr(n, i.length - 1);
        var found = -1;
        type = typeof prop;
        if(type === "string" &&
           language.length)
            found = prop.search(language);
        if(found !== -1)
        {
            text = item[i];
            break;
        }
    }

    if(text.length === 0)
        text = item[base];

    return text;
}

/** Function to extract items and add them to a QML based Combo box
 *
 *  param[in,out] combo          the combo box object containing
 *                               combo::key string,
 *                               combo::defaultValue string,
 *                               combo::combo ComboBox and
 *                               combo::model ListModel
 *  param[in]     choices        array which contains 'nick',
 *                               'name' and 'description' objects
 *                               in each array object + eventually
 *                               translations for 'name' and
 *                               'description'
 *                               [ {"nick": "shrt",
 *                                  "name": "medium",
 *                                  "name.de_DE": "kompakt"},
 *                                 {"nick": "shrt2",
 *                                  "name": "medium2",
 *                                  "description": "long texts"}
 *                               ]
 *  param[in]     key            combo meta data for DB storage
 *  param[in]     defaultValue   string that sets combo::defaultValue
 *  param[in]     loc            the language_country string,
 *                               e.g. "de_AT" for german language
 *                               and Austria region
 */
function setComboItems( combo, choices, key, defaultValue, loc )
{
    var current = -1;
    for( var index in choices )
    {
        if( typeof choices[index].name != "string" )
            continue;
        var item = choices[index];

        var name = getTranslatedItem( item, "name", loc );
        var desc = getTranslatedItem( item, "description", loc );

        var nick = item.nick;
        if(nick === defaultValue)
            current = name;
        combo.model.append({ "key": name, "nick": nick });
    }
    combo.key = key;
    if(typeof defaultValue !== "undefined")
    {
        combo.defaultValue = defaultValue;
        combo.currentValue = defaultValue;
    }
    combo.combo.currentIndex = combo.combo.find(current);
}
