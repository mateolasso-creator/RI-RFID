FROM php:8.1-apache

# Instalar extensiones de PHP necesarias para MySQL
RUN docker-php-ext-install mysqli pdo pdo_mysql

# Copiar los archivos PHP al directorio web
COPY src/php/ /var/www/html/

# Cambiar permisos si es necesario
RUN chown -R www-data:www-data /var/www/html/

# Exponer el puerto 80
EXPOSE 80