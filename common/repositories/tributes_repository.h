/**
 * EQEmulator: Everquest Server Emulator
 * Copyright (C) 2001-2020 EQEmulator Development Team (https://github.com/EQEmu/Server)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY except by those people which sell it, which
 * are required to give you total support for your newly bought product;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#ifndef EQEMU_TRIBUTES_REPOSITORY_H
#define EQEMU_TRIBUTES_REPOSITORY_H

#include "../database.h"
#include "../string_util.h"

class TributesRepository {
public:
	struct Tributes {
		int         id;
		int         unknown;
		std::string name;
		std::string descr;
		int8        isguild;
	};

	static std::string PrimaryKey()
	{
		return std::string("isguild");
	}

	static std::vector<std::string> Columns()
	{
		return {
			"id",
			"unknown",
			"name",
			"descr",
			"isguild",
		};
	}

	static std::string ColumnsRaw()
	{
		return std::string(implode(", ", Columns()));
	}

	static std::string InsertColumnsRaw()
	{
		std::vector<std::string> insert_columns;

		for (auto &column : Columns()) {
			if (column == PrimaryKey()) {
				continue;
			}

			insert_columns.push_back(column);
		}

		return std::string(implode(", ", insert_columns));
	}

	static std::string TableName()
	{
		return std::string("tributes");
	}

	static std::string BaseSelect()
	{
		return fmt::format(
			"SELECT {} FROM {}",
			ColumnsRaw(),
			TableName()
		);
	}

	static std::string BaseInsert()
	{
		return fmt::format(
			"INSERT INTO {} ({}) ",
			TableName(),
			InsertColumnsRaw()
		);
	}

	static Tributes NewEntity()
	{
		Tributes entry{};

		entry.id      = 0;
		entry.unknown = 0;
		entry.name    = "";
		entry.descr   = "";
		entry.isguild = 0;

		return entry;
	}

	static Tributes GetTributesEntry(
		const std::vector<Tributes> &tributess,
		int tributes_id
	)
	{
		for (auto &tributes : tributess) {
			if (tributes.isguild == tributes_id) {
				return tributes;
			}
		}

		return NewEntity();
	}

	static Tributes FindOne(
		int tributes_id
	)
	{
		auto results = content_db.QueryDatabase(
			fmt::format(
				"{} WHERE id = {} LIMIT 1",
				BaseSelect(),
				tributes_id
			)
		);

		auto row = results.begin();
		if (results.RowCount() == 1) {
			Tributes entry{};

			entry.id      = atoi(row[0]);
			entry.unknown = atoi(row[1]);
			entry.name    = row[2];
			entry.descr   = row[3];
			entry.isguild = atoi(row[4]);

			return entry;
		}

		return NewEntity();
	}

	static int DeleteOne(
		int tributes_id
	)
	{
		auto results = content_db.QueryDatabase(
			fmt::format(
				"DELETE FROM {} WHERE {} = {}",
				TableName(),
				PrimaryKey(),
				tributes_id
			)
		);

		return (results.Success() ? results.RowsAffected() : 0);
	}

	static int UpdateOne(
		Tributes tributes_entry
	)
	{
		std::vector<std::string> update_values;

		auto columns = Columns();

		update_values.push_back(columns[1] + " = " + std::to_string(tributes_entry.unknown));
		update_values.push_back(columns[2] + " = '" + EscapeString(tributes_entry.name) + "'");
		update_values.push_back(columns[3] + " = '" + EscapeString(tributes_entry.descr) + "'");

		auto results = content_db.QueryDatabase(
			fmt::format(
				"UPDATE {} SET {} WHERE {} = {}",
				TableName(),
				implode(", ", update_values),
				PrimaryKey(),
				tributes_entry.isguild
			)
		);

		return (results.Success() ? results.RowsAffected() : 0);
	}

	static Tributes InsertOne(
		Tributes tributes_entry
	)
	{
		std::vector<std::string> insert_values;

		insert_values.push_back(std::to_string(tributes_entry.unknown));
		insert_values.push_back("'" + EscapeString(tributes_entry.name) + "'");
		insert_values.push_back("'" + EscapeString(tributes_entry.descr) + "'");

		auto results = content_db.QueryDatabase(
			fmt::format(
				"{} VALUES ({})",
				BaseInsert(),
				implode(",", insert_values)
			)
		);

		if (results.Success()) {
			tributes_entry.id = results.LastInsertedID();
			return tributes_entry;
		}

		tributes_entry = TributesRepository::NewEntity();

		return tributes_entry;
	}

	static int InsertMany(
		std::vector<Tributes> tributes_entries
	)
	{
		std::vector<std::string> insert_chunks;

		for (auto &tributes_entry: tributes_entries) {
			std::vector<std::string> insert_values;

			insert_values.push_back(std::to_string(tributes_entry.unknown));
			insert_values.push_back("'" + EscapeString(tributes_entry.name) + "'");
			insert_values.push_back("'" + EscapeString(tributes_entry.descr) + "'");

			insert_chunks.push_back("(" + implode(",", insert_values) + ")");
		}

		std::vector<std::string> insert_values;

		auto results = content_db.QueryDatabase(
			fmt::format(
				"{} VALUES {}",
				BaseInsert(),
				implode(",", insert_chunks)
			)
		);

		return (results.Success() ? results.RowsAffected() : 0);
	}

	static std::vector<Tributes> All()
	{
		std::vector<Tributes> all_entries;

		auto results = content_db.QueryDatabase(
			fmt::format(
				"{}",
				BaseSelect()
			)
		);

		all_entries.reserve(results.RowCount());

		for (auto row = results.begin(); row != results.end(); ++row) {
			Tributes entry{};

			entry.id      = atoi(row[0]);
			entry.unknown = atoi(row[1]);
			entry.name    = row[2];
			entry.descr   = row[3];
			entry.isguild = atoi(row[4]);

			all_entries.push_back(entry);
		}

		return all_entries;
	}

	static std::vector<Tributes> GetWhere(std::string where_filter)
	{
		std::vector<Tributes> all_entries;

		auto results = content_db.QueryDatabase(
			fmt::format(
				"{} WHERE {}",
				BaseSelect(),
				where_filter
			)
		);

		all_entries.reserve(results.RowCount());

		for (auto row = results.begin(); row != results.end(); ++row) {
			Tributes entry{};

			entry.id      = atoi(row[0]);
			entry.unknown = atoi(row[1]);
			entry.name    = row[2];
			entry.descr   = row[3];
			entry.isguild = atoi(row[4]);

			all_entries.push_back(entry);
		}

		return all_entries;
	}

	static int DeleteWhere(std::string where_filter)
	{
		auto results = content_db.QueryDatabase(
			fmt::format(
				"DELETE FROM {} WHERE {}",
				TableName(),
				PrimaryKey(),
				where_filter
			)
		);

		return (results.Success() ? results.RowsAffected() : 0);
	}

};

#endif //EQEMU_TRIBUTES_REPOSITORY_H
