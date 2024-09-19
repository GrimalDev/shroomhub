FROM node:20-alpine

WORKDIR /app

RUN apk add --no-cache libc6-compat git

COPY package.json package-lock.json ./
RUN npm ci

RUN npm i -g next@latest

COPY . .
# RUN npm run build

# ENV NODE_ENV production
#
# # Disable Next.js telemetry
# ENV NEXT_TELEMETRY_DISABLED 1
#
# # Set correct permissions for nextjs user and don't run as root
# RUN addgroup nodejs
# RUN adduser -SDH nextjs
# RUN chown nextjs:nodejs .next

# USER nextjs

# Run the nextjs app
CMD ["npm", "run", "dev"]
