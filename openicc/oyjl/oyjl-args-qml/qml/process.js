/** @file process.js
 *
 *  OpenICC JSON QML is a graphical renderer of UI files.
 *
 *  @par Copyright:
 *            2018-2022 (C) Kai-Uwe Behrmann
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
 *  param[in]    catalog    org/freedektop/oyjl/translations {
                              "de_CH": {"string": "text",
                                        "string2": "text2"}}
 *  return                  nearest match for base.loc; e.g.:
 *                          base.de_AT or base.de_XX or base
 */
function getTranslatedItem( item, base, loc, catalog )
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

    // search in i18n catalog
    if( text.length === 0 &&
        typeof catalog === "object" )
    {
        var src = item[base]
        var sub_cat = catalog[loc]
        var tr;
        if(typeof sub_cat === "object")
            tr = catalog[loc][src]
        type = typeof tr;
        if(type !== "string")
        {
            sub_cat = catalog[language]
            if(typeof sub_cat === "object")
                tr = catalog[language][src]
        }
        type = typeof tr;
        if(type !== "string")
            for( i in catalog )
            {
                if(language.length)
                    found = i.search(language);
                if(found !== -1)
                    tr = catalog[i][src];
                if(typeof tr === "string")
                    break;
            }
        if(typeof tr === "string")
            text = tr
    }

    if(text.length === 0)
        text = item[base];

    return text;
}

/** Function to convert Oyjl term colors into HTML codes for QML */
function toHtml ( t )
{
    if(t.match(/^</)) // convert XML to HTML markup
    {
        t = t.replace(/</g, "&lt;")
        t = t.replace(/>/g, "&gt;")
    }

    if(t.match(/\033\[/)) // convert ansi color + format codes to HTML markup
    {
        t = t.replace(/\033\[1m/g, "<strong>")
        t = t.replace(/\033\[3m/g, "<em>")
        t = t.replace(/\033\[4m/g, "<u>")
        t = t.replace(/\033\[0m/g, "</u></strong></em></font>")
        t = t.replace(/\033\[00m/g, "</font>")
        t = t.replace(/ /g, '&nbsp;')
        t = t.replace(/\t/g, '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;')
        t = t.replace(/\n/g, "</font><br />")
        // some color codes
        t = t.replace(/\033\[31m/g, "<font color=red>")
        t = t.replace(/\033\[0;31m/g, "<font color=red>")
        t = t.replace(/\033\[00;31m/g, "<font color=red>")
        t = t.replace(/\033\[01;31m/g, "<font color=red>")
        t = t.replace(/\033\[38;2;240;0;0m/g, "<font color=red>")
        t = t.replace(/\033\[38;2;250;0;0m/g, "<font color=red>")
        t = t.replace(/\033\[32m/g, "<font color=green>")
        t = t.replace(/\033\[0;32m/g, "<font color=green>")
        t = t.replace(/\033\[00;32m/g, "<font color=green>")
        t = t.replace(/\033\[01;32m/g, "<font color=green>")
        t = t.replace(/\033\[38;2;0;250;100m/g, "<font color=green>")
        t = t.replace(/\033\[33m/g, "<font color=lime>")
        t = t.replace(/\033\[0;33m/g, "<font color=lime>")
        t = t.replace(/\033\[00;33m/g, "<font color=lime>")
        t = t.replace(/\033\[01;33m/g, "<font color=lime>")
        t = t.replace(/\033\[34m/g, "<font color=dodgerblue>")
        t = t.replace(/\033\[0;34m/g, "<font color=dodgerblue>")
        t = t.replace(/\033\[00;34m/g, "<font color=dodgerblue>")
        t = t.replace(/\033\[01;34m/g, "<font color=dodgerblue>")
        t = t.replace(/\033\[38;2;150;0;0m/g, "<font color=dodgerblue>")
        t = t.replace(/\033\[38;2;0;150;255m/g, "<font color=dodgerblue>")
        t = t.replace(/\033\[35m/g, "<font color=magenta>")
        t = t.replace(/\033\[0;35m/g, "<font color=magenta>")
        t = t.replace(/\033\[00;35m/g, "<font color=magenta>")
        t = t.replace(/\033\[01;35m/g, "<font color=magenta>")
        t = t.replace(/\033\[36m/g, "<font color=cyan>")
        t = t.replace(/\033\[0;36m/g, "<font color=cyan>")
        t = t.replace(/\033\[00;36m/g, "<font color=cyan>")
        t = t.replace(/\033\[01;36m/g, "<font color=cyan>")

        t = "<div style\"word-wrap:nowhere;\">" + t + "</div>"
    }
    return t;
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
function setComboItems( combo, choices, key, defaultValue, loc, catalog )
{
    var current = -1;
    var model = combo.model
    for( var index in choices )
    {
        if( typeof choices[index].name != "string" &&
            typeof choices[index].nick != "string" )
            continue;
        var item = choices[index];

        var name = getTranslatedItem( item, "name", loc, catalog );
        var desc = getTranslatedItem( item, "description", loc, catalog );

        var nick = item.nick;
        if( typeof name === "undefined" ||
           (typeof name === "string" && name.length === 0))
            name = nick;
        if(nick === defaultValue)
            current = name;
        model.append({ "key": name, "nick": nick });
    }
    combo.key = key;
    if(typeof defaultValue !== "undefined")
    {
        combo.defaultValue = defaultValue;
        combo.currentValue = defaultValue;
    }
    combo.combo.currentIndex = combo.combo.find(current);
}
