#!/usr/bin/env python3
"""
Growtopia Bot API Client Example

Contoh penggunaan API Growtopia Bot menggunakan Python.
Script ini mendemonstrasikan cara menggunakan berbagai endpoint API.
"""

import requests
import json
import time
from typing import Dict, Any, Optional

class GrowtopiaAPI:
    def __init__(self, base_url: str = "http://localhost:8080/api/1.0"):
        self.base_url = base_url
        self.headers = {"Content-Type": "application/json"}
    
    def _make_request(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Make HTTP POST request to API"""
        try:
            response = requests.post(self.base_url, json=data, headers=self.headers)
            return response.json()
        except requests.exceptions.RequestException as e:
            return {"success": False, "error": str(e)}
    
    # Bot Management
    def add_bot(self, username: str, password: str, server: str = "growtopia1.com", port: int = 17091) -> Dict[str, Any]:
        """Add a new bot to the pool"""
        return self._make_request({
            "option": "add",
            "tank_id_name": username,
            "tank_id_pass": password,
            "server": server,
            "port": port
        })
    
    def remove_bot(self, username: str) -> Dict[str, Any]:
        """Remove bot from pool"""
        return self._make_request({
            "option": "remove",
            "tank_id_name": username
        })
    
    def list_clients(self) -> Dict[str, Any]:
        """Get list of all active bots"""
        return self._make_request({"option": "list_clients"})
    
    def disconnect_bot(self, username: str) -> Dict[str, Any]:
        """Disconnect bot"""
        return self._make_request({
            "option": "disconnect",
            "tank_id_name": username
        })
    
    def reconnect_bot(self, username: str) -> Dict[str, Any]:
        """Reconnect bot"""
        return self._make_request({
            "option": "reconnect",
            "tank_id_name": username
        })
    
    # Movement
    def move_bot(self, username: str, x: int, y: int) -> Dict[str, Any]:
        """Move bot to coordinates"""
        return self._make_request({
            "option": "move",
            "tank_id_name": username,
            "x": x,
            "y": y
        })
    
    def teleport_bot(self, username: str, x: int, y: int) -> Dict[str, Any]:
        """Teleport bot to coordinates"""
        return self._make_request({
            "option": "teleport",
            "tank_id_name": username,
            "x": x,
            "y": y
        })
    
    def warp_bot(self, username: str, world: str) -> Dict[str, Any]:
        """Warp bot to world"""
        return self._make_request({
            "option": "warp",
            "tank_id_name": username,
            "world": world
        })
    
    # Block Interaction
    def place_block(self, username: str, item_id: int, x: int, y: int) -> Dict[str, Any]:
        """Place block at coordinates"""
        return self._make_request({
            "option": "place",
            "tank_id_name": username,
            "item_id": item_id,
            "x": x,
            "y": y
        })
    
    def punch_block(self, username: str, x: int, y: int) -> Dict[str, Any]:
        """Punch block at coordinates"""
        return self._make_request({
            "option": "punch",
            "tank_id_name": username,
            "x": x,
            "y": y
        })
    
    def wrench_block(self, username: str, x: int, y: int) -> Dict[str, Any]:
        """Use wrench on block"""
        return self._make_request({
            "option": "wrench",
            "tank_id_name": username,
            "x": x,
            "y": y
        })
    
    # Item Management
    def get_inventory(self, username: str) -> Dict[str, Any]:
        """Get bot inventory"""
        return self._make_request({
            "option": "inventory",
            "tank_id_name": username
        })
    
    def wear_item(self, username: str, item_id: int) -> Dict[str, Any]:
        """Wear item from inventory"""
        return self._make_request({
            "option": "wear",
            "tank_id_name": username,
            "item_id": item_id
        })
    
    def consume_item(self, username: str, item_id: int) -> Dict[str, Any]:
        """Consume item"""
        return self._make_request({
            "option": "consume",
            "tank_id_name": username,
            "item_id": item_id
        })
    
    def collect_items(self, username: str, range_val: int = 3, force: bool = False) -> Dict[str, Any]:
        """Collect items around bot"""
        return self._make_request({
            "option": "collect",
            "tank_id_name": username,
            "range": range_val,
            "force": force
        })
    
    # Information
    def get_client_info(self, username: str) -> Dict[str, Any]:
        """Get client information"""
        return self._make_request({
            "option": "client",
            "tank_id_name": username
        })
    
    def get_status(self, username: str) -> Dict[str, Any]:
        """Get bot status"""
        return self._make_request({
            "option": "status",
            "tank_id_name": username
        })
    
    def get_ping(self, username: str) -> Dict[str, Any]:
        """Get bot ping"""
        return self._make_request({
            "option": "ping",
            "tank_id_name": username
        })
    
    def get_world_info(self, username: str) -> Dict[str, Any]:
        """Get world information"""
        return self._make_request({
            "option": "world_info",
            "tank_id_name": username
        })
    
    def get_player_info(self, username: str) -> Dict[str, Any]:
        """Get player information"""
        return self._make_request({
            "option": "player_info",
            "tank_id_name": username
        })
    
    # Communication
    def send_message(self, username: str, message: str) -> Dict[str, Any]:
        """Send message"""
        return self._make_request({
            "option": "send",
            "tank_id_name": username,
            "message": message
        })
    
    def smoke(self, username: str) -> Dict[str, Any]:
        """Send smoke to random player"""
        return self._make_request({
            "option": "smoke",
            "tank_id_name": username
        })
    
    # Utility
    def reset_bot(self, username: str) -> Dict[str, Any]:
        """Reset bot to initial state"""
        return self._make_request({
            "option": "reset",
            "tank_id_name": username
        })


def main():
    """Example usage of the API"""
    api = GrowtopiaAPI()
    
    # Example bot credentials (replace with actual credentials)
    bot_username = "39JJ"
    bot_password = "Pr@setyo123"
    
    print("=== Growtopia Bot API Example ===")
    
    # 1. List current clients
    print("\n1. Listing current clients...")
    result = api.list_clients()
    print(json.dumps(result, indent=2))
    

    # 2. Add a bot (uncomment to use)
    # print(f"\n2. Adding bot '{bot_username}'...")
    result = api.add_bot(bot_username, bot_password)
    # print(json.dumps(result, indent=2))
    
    api.reconnect_bot(bot_username)
    # 3. Get bot info (if bot exists)
    print(f"\n3. Getting info for bot '{bot_username}'...")
    result = api.get_client_info(bot_username)
    print(json.dumps(result, indent=2))
    
    # 4. Get inventory
    print(f"\n4. Getting inventory for bot '{bot_username}'...")
    result = api.get_inventory(bot_username)
    print(json.dumps(result, indent=2))
    
    # 5. Move bot
    print(f"\n5. Moving bot '{bot_username}' to (100, 200)...")
    result = api.move_bot(bot_username, 100, 200)
    print(json.dumps(result, indent=2))
    
    # 6. Get player info
    print(f"\n6. Getting player info for bot '{bot_username}'...")
    result = api.get_player_info(bot_username)
    print(json.dumps(result, indent=2))
    
    print("\n=== Example completed ===")


if __name__ == "__main__":
    main()