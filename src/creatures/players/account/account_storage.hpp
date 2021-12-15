/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (C) 2021 OpenTibiaBR <opentibiabr@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef SRC_CREATURES_PLAYERS_ACCOUNT_ACCOUNT_STORAGE_HPP_
#define SRC_CREATURES_PLAYERS_ACCOUNT_ACCOUNT_STORAGE_HPP_

#include <vector>


namespace account_storage
{

/**
 * @brief Account class to handle account information storage
 *
 */
class AccountStorage
{
public:

    virtual error_t loadAccountByID() = 0;
    virtual error_t loadAccountByName() = 0;

    virtual error_t saveAccount() = 0;

    virtual error_t loadAccountPlayerDB() = 0;
    virtual error_t loadAccountPlayersDB() =  0;


    virtual error_t getCoins() = 0;
    virtual error_t addCoins(const uint32_t& amount) = 0;
    virtual error_t removeCoins(const uint32_t& amount) = 0;
    virtual error_t registerCoinsTransaction() = 0;
};

} // namespace account_storage

#endif // SRC_CREATURES_PLAYERS_ACCOUNT_ACCOUNT_STORAGE_HPP_
